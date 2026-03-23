/**
 * @copyright 2026 David Shurgold <aomdoa@gmail.com>
 */
#ifndef SCREEN_CONTROL_H
#define SCREEN_CONTROL_H
#include "config.h"

typedef struct {
  ScreenConfig *config;
  ScreenType *type;
  char *error;
} ScreenLookup;

typedef struct {
  char *command;
  char *rawResponse;
  char *response;
  char *error;
} ScreenResult;

char *get_screens();
ScreenResult get_screen_status(const char *);
ScreenResult set_screen_status(const char *, int);

#endif /* SCREEN_CONTROL_H */