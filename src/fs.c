#include "fs.h"

#include <stdio.h>
#include <stdlib.h>

char *fs_read_as_text(char const *filename)
{
  FILE *file = fopen(filename, "r");
  if (!file)
  {
    perror("Cannot read file");
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  rewind(file);

  char *buffer = malloc(size);
  if (buffer)
  {
    size_t read = fread(buffer, 1, size, file);
    buffer[read] = 0;
  }

  fclose(file);

  return buffer;
}
