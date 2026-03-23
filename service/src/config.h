/**
 * @copyright 2026 David Shurgold <aomdoa@gmail.com>
 */
#ifndef CONFIG_H
#define CONFIG_H

typedef struct
{
  char *id;
  char *name;
  char *protocol;
  char *display_status;
  char *display_on;
  char *display_off;
} ScreenType;

typedef struct
{
  char *id;
  char *name;
  char *type;
  char *address;
  int port;
} ScreenConfig;

typedef struct
{
  ScreenType *screen_types;
  int screen_type_count;
  ScreenConfig *screen_configs;
  int screen_config_count;
} Configuration;

Configuration *get_configurations();
ScreenConfig *get_screen_config(const char *);
ScreenType *get_screen_type(const char *);

#endif /* CONFIG_H */