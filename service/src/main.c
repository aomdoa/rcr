/**
 * @copyright 2026 David Shurgold <aomdoa@gmail.com>
 */
#include <stdio.h>
#include "service.h"
#include "config.h"

int main()
{
  get_configurations(); // ensure it's setup properly
  struct MHD_Daemon *d = service_start();
  if (!d)
    return 1;
  printf("STARTED!\n");
  getchar();
  service_stop(d);
  return 0;
}
