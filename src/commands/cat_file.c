
#include <stdio.h>
#include <string.h>

#include "git/objects.h"

void cat_file(int argc, char *argv[]) {
  if (argc < 4) {
    fprintf(stderr, "Usage: ./your_program.sh cat-file [<args>] <object>\n");
    return;
  }

  char *param = argv[2];
  char *object_name = argv[3];

  if (strcmp(argv[2], "-p") != 0) {
    fprintf((stderr), "Argument not implemented");
    return;
  }

  FILE *object_contents = read_git_object_from_sha(object_name);

  do {
    char c = fgetc(object_contents);

    if (c == '\0') {
      break;
    }
  } while (1);

  do {
    char c = fgetc(object_contents);

    if (c == EOF) {
      break;
    }

    printf("%c", c);
  } while (1);

  fclose(object_contents);
}
