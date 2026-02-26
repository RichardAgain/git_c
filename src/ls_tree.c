
#include <linux/limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "objects.h"

typedef struct {
  char **data;
  size_t count;
} FileNames;

int compare(const void *a, const void *b) {
  return strcmp(*(char **)a, *(char **)b);
}

void read_tree_object(FILE *fptr) {
  FileNames file_names = {};
  char **tmp;

  char object_name[FILENAME_MAX];
  char object_mode[7];

  size_t i;
  char c;

  while ((c = fgetc(fptr)) != 0) {
  }

  while (fread(object_mode, 1, 6, fptr) != 0) {
    char *cptr = (char *)memchr(object_mode, ' ', 6);

    if (cptr != NULL) {
      *cptr = 0;
    } else {
      fseek(fptr, 1, SEEK_CUR);
    }

    memset(object_name, '\0', FILENAME_MAX);

    i = 0;
    while ((c = fgetc(fptr)) != '\0') {
      object_name[i] = c;
      i++;
    }

    tmp = realloc(file_names.data, (file_names.count + 1) * sizeof(char *));
    if (tmp == NULL) {
      printf("reallocation failed");
    }
    file_names.data = tmp;
    file_names.data[file_names.count++] = strdup(object_name);

    fseek(fptr, 20, SEEK_CUR);
  }

  qsort(file_names.data, file_names.count, sizeof(char *), compare);

  for (int j = 0; j < file_names.count; j++) {
    printf("%s\n", file_names.data[j]);
  }

  free(file_names.data);
}

void ls_tree(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(stderr, "Usage: ./your_program.sh cat-file [<args>] <object>\n");
    return;
  }

  char *object_sha = argv[2];

  FILE *object_contents = read_git_object_from_sha(object_sha);

  read_tree_object(object_contents);

  fclose(object_contents);
}