#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "git/files.h"
#include "git/objects.h"

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

  char *object_hex = argv[2];
  if (strlen(object_hex) != 40) {
    printf("Not a valid object name\n");
    return;
  }

  file_result_t fr = read_file_from_hex(object_hex);
  if (fr.file == NULL) {
    printf("Error reading file\n");
    return;
  }

  GitObject *object = parse_git_object(fr.file, fr.size);
  git_tree_t *tree = parse_git_tree(object);

  print_tree(tree);
}
