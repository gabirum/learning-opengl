#include "fs.h"

#include <stdio.h>
#include <stdlib.h>

uint8_t *fs_read_all_file(char const *filename)
{
  FILE *file = fopen(filename, "rb");
  if (file == NULL)
  {
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  rewind(file);

  uint8_t *data = calloc(size + 1, 1);
  if (data == NULL)
  {
    goto close;
  }

  fread(data, 1, size, file);

close:
  fclose(file);

  return data;
}
