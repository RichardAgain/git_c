#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "git/files.h"
#include "git/objects.h"

void hash_object(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(stderr, "Usage: ./your_program.sh hash-object [<args>] <object>\n");
    return;
  }

  char *param = argv[2];
  char *file_path = argv[3];

  if (strcmp(argv[2], "-w") != 0) {
    fprintf((stderr), "Argument not implemented");
    return;
  }

  file_result_t fr = read_file_from_path(file_path);

  unsigned char *contents = calloc(fr.size, sizeof(unsigned char));
  if (contents == NULL)
    return;

  fread(contents, sizeof(unsigned char), fr.size, fr.file);

  FILE *temp = tmpfile();
  if (temp == NULL) {
    perror("tmpfile");
    return;
  }

  GitObject *object = write_git_object(temp, contents, fr.size);

  printf("%s\n", object->sha1_hex);

  fr = write_file_from_hex(object->sha1_hex, temp);

  free(contents);
  fclose(temp);
}
