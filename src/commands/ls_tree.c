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

// void read_tree_object(GitObject *tree) {
//   FileNames file_names = {};
//   char **tmp;

//   char object_name[FILENAME_MAX];
//   size_t i;
//   char c;

//   qsort(file_names.data, file_names.count, sizeof(char *), compare);

//   for (int j = 0; j < file_names.count; j++) {
//     printf("%s\n", file_names.data[j]);
//   }

//   for (size_t j = 0; j < file_names.count; j++) {
//     free(file_names.data[j]);
//   }
//   free(file_names.data);
// }

void ls_tree(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(stderr, "Usage: ./your_program.sh cat-file [<args>] <object>\n");
    return;
  }

  char *object_sha = argv[2];

  GitObject *object = read_git_object_from_sha(object_sha);

  git_tree_t *tree = read_git_tree(object);

  for (int i = 0; i < tree->length; i++) {
    printf("%s %s %s\n", tree->entries[i].mode, tree->entries[i].hex,
           tree->entries[i].name);
  }
}
