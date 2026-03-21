/**
 * @copyright 2026 David Shurgold <aomdoa@gmail.com>
 */
#include <cjson/cJSON.h>
#include "screen_control.h"
#include "config.h"

char *getScreens()
{
  Configuration *config = getConfigurations();
  cJSON *screens = cJSON_CreateArray();
  for (int i = 0; i < config->screen_config_count; i++) {
    cJSON *screen = cJSON_CreateObject();
    cJSON_AddStringToObject(screen, "id", config->screen_configs[i].id);
    cJSON_AddStringToObject(screen, "name", config->screen_configs[i].name);
    cJSON_AddStringToObject(screen, "type", config->screen_configs[i].type);
    cJSON_AddStringToObject(screen, "address", config->screen_configs[i].address);
    cJSON_AddNumberToObject(screen, "id", config->screen_configs[i].port);
    cJSON_AddItemToArray(screens, screen);
  }
  char *string = cJSON_Print(screens);
  cJSON_Delete(screens);
  return string;
}

char *getScreenStatus(char *);
char *setScreenStatus(char *, int);