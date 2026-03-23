/**
 * @copyright 2026 David Shurgold <aomdoa@gmail.com>
 */
#include <cjson/cJSON.h>
#include <stdio.h>
#include "screen_control.h"

char *get_screens()
{
  Configuration *config = get_configurations();
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

ScreenLookup get_screen_and_type(const char *screenId) {
  ScreenLookup result = {NULL, NULL, NULL};
  result.config = get_screen_config(screenId);
  if (result.config == NULL) {
    result.error = "Invalid Screen";
    return result;
  }
  result.type = get_screen_type(result.config->type);
  if (result.type == NULL) {
    result.error = "Screen has invalid type";
  }
  return result;
}

//TODO - should we instead return boolean and have consume setup the blob for data?
ScreenResult get_screen_status(const char *screenId) {
  ScreenResult result = {NULL, NULL, NULL, NULL};
  ScreenLookup screen = get_screen_and_type(screenId);
  if (screen.error) {
    result.error = screen.error;
    return result;
  }
  char response[128];
  // TODO actually build the serial control
  //int send_command(const char *address, int port, const char *command, size_t cmd_len, char *response, size_t resp_len)
  fprintf(stdout, "send_command(%s, %d, %s, %d, response, %d)\n", screen.config->address, screen.config->port, screen.type->display_status, sizeof(screen.type->display_status), sizeof(response));
  result.rawResponse = "AA 00 XX bp f"; // we get response, determine status and include raw, processed and flagged
  result.response = "ON"; // json? status is just on/off but getting deeper can have a lot more info, maybe type it
  result.command = screen.type->display_status; // testing with just hardcode for now
  return result;
}

ScreenResult set_screen_status(const char *screenId, int power) {
  ScreenResult result = {NULL, NULL, NULL, NULL};
  ScreenLookup screen = get_screen_and_type(screenId);
  if (screen.error) {
    result.error = screen.error;
    return result;
  }
  char response[128];
  result.command = (power == 0) ? screen.type->display_off : screen.type->display_on;
  fprintf(stdout, "send_command(%s, %d, %s, %d, response, %d)\n", screen.config->address, screen.config->port, result.command, sizeof(result.command), sizeof(response));
  result.rawResponse = "ka 01 01";
  result.response = (power == 0) ? "OFF" : "ON";
  return result;
}