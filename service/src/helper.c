#include "helper.h"
#include <stdio.h>
#include <stdlib.h>

char *read_file(const char *path)
{
  FILE *f = fopen(path, "rb");
  if (!f) {
    fprintf(stderr, "Failed to open config file: %s\n", path);
    return NULL;
  }
  fseek(f, 0, SEEK_END);
  long length = ftell(f);
  fseek(f, 0, SEEK_SET);
  char *data = malloc(length + 1);
  fread(data, 1, length, f);
  fclose(f);
  data[length] = '\0';
  return data;
}