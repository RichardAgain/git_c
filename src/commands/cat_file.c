
#include <stdio.h>
#include <string.h>

#include "git/files.h"
#include "git/objects.h"

void cat_file(int argc, char *argv[]) {
  if (argc < 4) {
    fprintf(stderr, "Usage: ./your_program.sh cat-file [<args>] <object>\n");
    return;
  }

  char *param = argv[2];
  char *object_hex = argv[3];

  if (strcmp(argv[2], "-p") != 0) {
    fprintf((stderr), "Argument not implemented");
    return;
  }

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
  if (object == NULL) {
    printf("Error parsing object\n");
    return;
  }

  if (strcmp(object->header.type_s, "blob") != 0) {
    printf("Object type not implemented: %s", object->header.type_s);
  }

  printf("%s", object->contents);
}
