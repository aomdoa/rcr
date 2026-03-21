#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "service.h"
#include "screen_control.h"

#define BODY_INIT_CAP 256 /* initial body buffer allocation            */
#define BODY_MAX 8192     /* reject payloads larger than this          */

// context - every request
ServiceRequestContext *service_request_context_new(void)
{
  ServiceRequestContext *ctx = calloc(1, sizeof(ServiceRequestContext));
  if (!ctx)
    return NULL;

  ctx->body = malloc(BODY_INIT_CAP);
  if (!ctx->body)
  {
    free(ctx);
    return NULL;
  }

  ctx->body[0] = '\0';
  ctx->body_len = 0;
  ctx->body_cap = BODY_INIT_CAP;
  return ctx;
}

enum MHD_Result service_request_context_append(
    ServiceRequestContext *ctx,
    const char *data,
    size_t size)
{
  if (!ctx || !data || size == 0)
    return MHD_YES;

  /* Guard against oversized payloads before touching the buffer */
  if (ctx->body_len + size > BODY_MAX)
  {
    fprintf(stderr, "service: request body exceeds %d-byte limit\n", BODY_MAX);
    return MHD_NO;
  }

  size_t needed = ctx->body_len + size + 1; /* +1 for NUL */
  if (needed > ctx->body_cap)
  {
    size_t new_cap = ctx->body_cap * 2;
    while (new_cap < needed)
      new_cap *= 2;

    char *resized = realloc(ctx->body, new_cap);
    if (!resized)
      return MHD_NO;

    ctx->body = resized;
    ctx->body_cap = new_cap;
  }

  memcpy(ctx->body + ctx->body_len, data, size);
  ctx->body_len += size;
  ctx->body[ctx->body_len] = '\0';
  return MHD_YES;
}

void service_request_context_free(ServiceRequestContext *ctx)
{
  if (!ctx)
    return;
  free(ctx->body);
  free(ctx);
}

enum MHD_Result service_send_json(
    struct MHD_Connection *connection,
    unsigned int status_code,
    const char *json)
{
  size_t len = strlen(json);
  struct MHD_Response *resp = MHD_create_response_from_buffer(
      len, (void *)json, MHD_RESPMEM_MUST_COPY);

  if (!resp)
    return MHD_NO;

  MHD_add_response_header(resp, "Content-Type", "application/json");

  enum MHD_Result ret = MHD_queue_response(connection, status_code, resp);
  MHD_destroy_response(resp);
  return ret;
}

// gets/patch
enum MHD_Result handle_get_screen_status(struct MHD_Connection *connection, const char *name)
{
  /* TODO: look up `name` in your screen registry */
  char output[256];
  snprintf(output, sizeof(output), "{\"name\": \"%s\", \"status\": \"active\"}", name);
  fprintf(stdout, "service: GET /screenstatus/%s result %s\n", name, output);
  return service_send_json(connection, MHD_HTTP_OK, output);
}

enum MHD_Result handle_patch_screen_status(
    struct MHD_Connection *connection,
    const char *id,
    const char *body,
    size_t body_len)
{
  char output[256];
  if (!body || body_len == 0)
    return service_send_json(connection, MHD_HTTP_BAD_REQUEST,
                             "{\"error\": \"empty body\"}");

  /* TODO: parse body (e.g. with cJSON), validate fields, apply update */
  fprintf(stdout, "service: PATCH /screenstatus/%s body: %.*s\n",
          (int)body_len, id, body);

  snprintf(output, sizeof(output), "{\"id\": \"%s\", \"status\": \"success\"}", id);
  return service_send_json(connection, MHD_HTTP_OK, output);
}

enum MHD_Result handle_get_screens(struct MHD_Connection *connection)
{
  char *screens = getScreens();
  return service_send_json(connection, MHD_HTTP_OK, screens);
}

// router
enum MHD_Result service_request_handler(
    void *cls,
    struct MHD_Connection *connection,
    const char *url,
    const char *method,
    const char *version,
    const char *upload_data,
    size_t *upload_data_size,
    void **con_cls)
{
  (void)cls;
  (void)version;

  /* ── First call: allocate per-request context ── */
  if (*con_cls == NULL)
  {
    ServiceRequestContext *ctx = service_request_context_new();
    if (!ctx)
      return MHD_NO;
    *con_cls = ctx;
    return MHD_YES;
  }

  ServiceRequestContext *ctx = (ServiceRequestContext *)*con_cls;

  /* ── Accumulate body chunks for methods that carry a payload ── */
  if (*upload_data_size > 0)
  {
    if (service_request_context_append(ctx, upload_data, *upload_data_size) == MHD_NO)
      return MHD_NO;
    *upload_data_size = 0; /* signal to MHD that we consumed the chunk */
    return MHD_YES;
  }

  /* GET /screenstatus/:name */
  if (strcmp(method, "GET") == 0 &&
      strncmp(url, "/screenstatus/", 14) == 0)
  {
    const char *name = url + 14;
    if (*name == '\0')
      return service_send_json(connection, MHD_HTTP_BAD_REQUEST, "{\"error\": \"missing screen name\"}");
    return handle_get_screen_status(connection, name);
  }

  /* PATCH /screenstatus/:name */
  if (strcmp(method, "PATCH") == 0 &&
      strncmp(url, "/screenstatus/", 14) == 0)
  {
    const char *name = url + 14;
    if (*name == '\0')
      return service_send_json(connection, MHD_HTTP_BAD_REQUEST, "{\"error\": \"missing screen name\"}");
    return handle_patch_screen_status(connection, name, ctx->body, ctx->body_len);
  }

  /* GET /screens */
  if (strcmp(method, "GET") == 0 && strcmp(url, "/screens") == 0)
    return handle_get_screens(connection);

  /* 404 fallback */
  return service_send_json(connection, MHD_HTTP_NOT_FOUND,
                           "{\"error\": \"not found\"}");
}

// clean up
static void on_request_completed(
    void *cls,
    struct MHD_Connection *connection,
    void **con_cls,
    enum MHD_RequestTerminationCode toe)
{
  (void)cls;
  (void)connection;
  (void)toe;

  service_request_context_free((ServiceRequestContext *)*con_cls);
  *con_cls = NULL;
}

// service
struct MHD_Daemon *service_start(void)
{
  struct MHD_Daemon *daemon = MHD_start_daemon(
      MHD_USE_INTERNAL_POLLING_THREAD,
      SERVICE_PORT,
      NULL, NULL,
      &service_request_handler, NULL,
      MHD_OPTION_NOTIFY_COMPLETED,
      on_request_completed, NULL,
      MHD_OPTION_END);

  if (!daemon)
  {
    fprintf(stderr, "service: failed to start daemon on port %d\n",
            SERVICE_PORT);
    return NULL;
  }

  fprintf(stdout, "service: listening on :%d\n", SERVICE_PORT);
  return daemon;
}

void service_stop(struct MHD_Daemon *daemon)
{
  if (!daemon)
    return;
  MHD_stop_daemon(daemon);
  fprintf(stdout, "service: stopped\n");
}