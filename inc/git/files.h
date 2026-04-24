
#include <stddef.h>
#include <stdio.h>

#define OBJECT_PATH_LENGTH 56

typedef struct {
  FILE *file;
  size_t size;
} file_result_t;

file_result_t read_file_from_hex(char *sha_hex);
file_result_t write_file_from_hex(char *sha_hex);
