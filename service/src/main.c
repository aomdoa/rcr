#include <stdio.h>
#include "service.h"

int main()
{
  
  struct MHD_Daemon *d = service_start();
  if (!d)
    return 1;
  printf("STARTED!\n");
  getchar();
  service_stop(d);
  return 0;
}
