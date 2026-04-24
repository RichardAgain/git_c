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

void print_tree(git_tree_t *tree) {
  for (int i = 0; i < tree->length; i++) {
    printf("%s\n", tree->entries[i].name);
  }
}

void ls_tree(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(stderr, "Usage: ./your_program.sh ls-tree [<args>] <object>\n");
    return;
  }

  char *object_sha = argv[2];

  GitObject *object = read_git_object_from_sha(object_sha);
  git_tree_t *tree = parse_git_tree(object);

  print_tree(tree);
}
