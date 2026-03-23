/**
 * @copyright 2026 David Shurgold <aomdoa@gmail.com>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>
#include "config.h"
#include "helper.h"

int get_number_value(const cJSON *element, const char *name)
{
  const cJSON *value = cJSON_GetObjectItemCaseSensitive(element, name);
  if (!cJSON_IsNumber(value))
  {
    fprintf(stderr, "Error getting number value from %s\n", name);
    return -1;
  }
  return value->valueint;
}

char *get_string_value(const cJSON *element, const char *name)
{
  const cJSON *value = cJSON_GetObjectItemCaseSensitive(element, name);
  if (!cJSON_IsString(value))
  {
    fprintf(stderr, "Error getting string value from %s\n", name);
    return NULL;
  }
  return strdup(value->valuestring);
}

static Configuration *_config = NULL;
Configuration *get_configurations()
{
  if (_config) {
    return _config;
  }
  // load and parse
  char *config_raw = read_file("config.json");
  if (!config_raw)
    return NULL;
  cJSON *config_json = cJSON_Parse(config_raw);
  free(config_raw);
  if (!config_json)
  {
    fprintf(stderr, "Error processing json from config.json\n");
    return NULL;
  }

  // now build our configuration
  Configuration *config = malloc(sizeof(Configuration));
  
  cJSON *screen_types = cJSON_GetObjectItemCaseSensitive(config_json, "screen_types");
  config->screen_type_count = cJSON_GetArraySize(screen_types);
  config->screen_types = malloc(sizeof(ScreenType) * config->screen_type_count);

  int i = 0;
  const cJSON *st = NULL;
  cJSON_ArrayForEach(st, screen_types)
  {
    config->screen_types[i].id = strdup(get_string_value(st, "id"));
    config->screen_types[i].name = strdup(get_string_value(st, "name"));
    config->screen_types[i].protocol = strdup(get_string_value(st, "protocol"));
    config->screen_types[i].display_status = strdup(get_string_value(st, "display_status"));
    config->screen_types[i].display_on = strdup(get_string_value(st, "display_on"));
    config->screen_types[i].display_off = strdup(get_string_value(st, "display_off"));
    i++;
  }

  // --- screen_configs ---
  cJSON *screen_configs = cJSON_GetObjectItemCaseSensitive(config_json, "screens");
  config->screen_config_count = cJSON_GetArraySize(screen_configs);
  config->screen_configs = malloc(sizeof(ScreenConfig) * config->screen_config_count);

  i = 0;
  const cJSON *sc = NULL;
  cJSON_ArrayForEach(sc, screen_configs)
  {
    config->screen_configs[i].id = strdup(get_string_value(sc, "id"));
    config->screen_configs[i].name = strdup(get_string_value(sc, "name"));
    config->screen_configs[i].type = strdup(get_string_value(sc, "type"));
    config->screen_configs[i].address = strdup(get_string_value(sc, "address"));
    config->screen_configs[i].port = get_number_value(sc, "port");
    i++;
  }

  cJSON_Delete(config_json);
  _config = config;
  return config;
}

ScreenConfig *get_screen_config(const char *id) {
  Configuration *config = get_configurations();
  for (int i = 0; i < config->screen_config_count; i++) {
    if (strcmp(id, config->screen_configs[i].id) == 0) {
      return &config->screen_configs[i];
    }
  }
  return NULL;
}

ScreenType *get_screen_type(const char *id) {
  Configuration *config = get_configurations();
  for (int i = 0; i< config->screen_type_count; i++) {
    if (strcmp(id, config->screen_types[i].id) == 0) {
      return &config->screen_types[i];
    }
  }
  return NULL;
}