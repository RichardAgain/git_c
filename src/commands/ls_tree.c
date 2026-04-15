
#include <linux/limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "git/objects.h"

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
  size_t i;
  char c;

  while ((c = fgetc(fptr)) != 0) {
  }

  while ((c = fgetc(fptr)) != EOF) {
    if (c == '\0') {
      break;
    }

    while (c != ' ') {
      c = fgetc(fptr);
      if (c == EOF) {
        return;
      }
    }

    memset(object_name, '\0', FILENAME_MAX);
    i = 0;
    while ((c = fgetc(fptr)) != '\0') {
      if (i + 1 < FILENAME_MAX) {
        object_name[i++] = c;
      }
    }

    tmp = realloc(file_names.data, (file_names.count + 1) * sizeof(char *));
    if (tmp == NULL) {
      fprintf(stderr, "reallocation failed\n");
      return;
    }
    file_names.data = tmp;
    file_names.data[file_names.count++] = strdup(object_name);

    if (fseek(fptr, 20, SEEK_CUR) != 0) {
      break;
    }
  }

  qsort(file_names.data, file_names.count, sizeof(char *), compare);

  for (int j = 0; j < file_names.count; j++) {
    printf("%s\n", file_names.data[j]);
  }

  for (size_t j = 0; j < file_names.count; j++) {
    free(file_names.data[j]);
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
