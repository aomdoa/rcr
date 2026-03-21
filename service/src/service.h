#ifndef SERVICE_H
#define SERVICE_H

#include <microhttpd.h>
#include <stddef.h>

#define SERVICE_PORT 8080

/**
 * Start the HTTP daemon on SERVICE_PORT.
 * Returns the daemon handle on success, NULL on failure.
 */
struct MHD_Daemon *service_start(void);

/**
 * Stop a running daemon and release its resources.
 */
void service_stop(struct MHD_Daemon *daemon);

/**
 * Top-level MHD request callback — routes every incoming request.
 * Register this as the request handler when calling MHD_start_daemon().
 */
enum MHD_Result service_request_handler(
    void                 *cls,
    struct MHD_Connection *connection,
    const char           *url,
    const char           *method,
    const char           *version,
    const char           *upload_data,
    size_t               *upload_data_size,
    void                **con_cls
);

 
/**
 * GET /screenstatus/:name
 * Returns the status of the named screen as JSON.
 * `name` is extracted from the URL path and is guaranteed non-NULL.
 */
enum MHD_Result handle_get_screen_status(
    struct MHD_Connection *connection,
    const char            *name
);

/**
 * PATCH /screenstatus/:name
 * Accepts a JSON body and applies a status update.
 * `name`      - is extracted from the URL path
 * `body`      — accumulated request body buffer.
 * `body_len`  — length of that buffer in bytes.
 */
enum MHD_Result handle_patch_screen_status(
    struct MHD_Connection *connection,
    const char            *name,
    const char            *body,
    size_t                 body_len
);

/**
 * GET /screens
 * Returns the list of known screens as a JSON array.
 */
enum MHD_Result handle_get_screens(struct MHD_Connection *connection);

/**
 * Build and enqueue a JSON response in one call.
 *
 * @param connection  Active MHD connection.
 * @param status_code HTTP status code (e.g. MHD_HTTP_OK).
 * @param json        NULL-terminated JSON string to send.
 *
 * The buffer is copied internally (MHD_RESPMEM_MUST_COPY), so `json`
 * may be a stack-allocated or temporary string.
 */
enum MHD_Result service_send_json(
    struct MHD_Connection *connection,
    unsigned int           status_code,
    const char            *json
);

/**
 * Heap-allocated state created on the first MHD callback for a request
 * and freed via service_request_context_free() on completion.
 *
 * Attach to *con_cls so the router can accumulate an upload body across
 * multiple callback invocations before dispatching to a handler.
 */
typedef struct {
    char   *body;       /* accumulated upload data   */
    size_t  body_len;   /* current length in bytes   */
    size_t  body_cap;   /* allocated capacity        */
} ServiceRequestContext;

/**
 * Allocate and zero-initialise a new ServiceRequestContext.
 * Returns NULL on allocation failure.
 */
ServiceRequestContext *service_request_context_new(void);

/**
 * Append `size` bytes from `data` to the context's body buffer,
 * growing the allocation as needed.
 * Returns MHD_YES on success, MHD_NO on allocation failure.
 */
enum MHD_Result service_request_context_append(
    ServiceRequestContext *ctx,
    const char            *data,
    size_t                 size
);

/**
 * Free a ServiceRequestContext and its internal body buffer.
 * Safe to call with NULL.
 */
void service_request_context_free(ServiceRequestContext *ctx);

#endif /* SERVICE_H */
