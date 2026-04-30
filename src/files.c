#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <zlib.h>

#include "git/files.h"
#include "git/zpipe.h"

void get_file_path_from_hex(char *object_path, char *object_hex) {
  sprintf(object_path, ".git/objects/%c%c/%s", object_hex[0], object_hex[1],
          object_hex + 2);
}

file_result_t read_file_from_path(char *path) {

  file_result_t fr = {
      .file = NULL,
      .size = 0,
  };

  FILE *file = fopen(path, "rb");

  if (file == NULL) {
    fprintf((stderr), "File not found");
    return fr;
  }

  fseek(file, 0, SEEK_END);
  size_t content_size = ftell(file);
  rewind(file);

  fr.file = file;
  fr.size = content_size;

  return fr;
}

file_result_t read_file_from_hex(char *sha_hex) {
  char path[GIT_OBJECT_PATH_LENGTH];
  get_file_path_from_hex(path, sha_hex);

  file_result_t fr = {
      .file = NULL,
      .size = 0,
  };

  FILE *temp = tmpfile();
  FILE *object_file = fopen(path, "rb");
  if (object_file == NULL || temp == NULL) {
    perror("read file");
    return fr;
  }

  inf(object_file, temp);

  size_t total_size = ftell(temp);
  if (total_size < 0) {
    perror("ftell temp");
    fclose(object_file);
    fclose(temp);
    return fr;
  }

  rewind(temp);
  fclose(object_file);

  fr.file = temp;
  fr.size = total_size;

  return fr;
}

file_result_t write_file_from_hex(char *sha_hex, FILE *input_f) {
  file_result_t fr = {0};

  char path[GIT_OBJECT_PATH_LENGTH];
  get_file_path_from_hex(path, sha_hex);

  char folder_path[16];
  strncpy(folder_path, path, 16);
  folder_path[15] = 0;

  printf("%s\n", folder_path);

  if (mkdir(folder_path, 0777) != 0) {
    // printf("Didnt create folder");
  };

  FILE *object_f = fopen(path, "w+b");

  if (object_f == NULL) {
    perror("fopen");
    return fr;
  }

  rewind(input_f);
  def(input_f, object_f, Z_DEFAULT_COMPRESSION);
  size_t content_size = ftell(input_f);

  fr.file = object_f;
  fr.size = content_size;

  return fr;
};