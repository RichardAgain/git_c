
#include "lib/zpipe.h"
#include <stdio.h>
#include <string.h>

void cat_file(int argc, char *argv[]) {

  if (argc < 4) {
    fprintf(stderr, "Usage: ./your_program.sh cat-file [<args>] <object>\n");
    return;
  }

  const char *param = argv[2];
  const char *object_name = argv[3];

  if (strcmp(argv[2], "-p") != 0) {
    fprintf((stderr), "Argument not implemented");
    return;
  }

  char object_path[200];
  sprintf(object_path, ".git/objects/%c%c/%s", object_name[0], object_name[1],
          object_name + 2);

  FILE *cfile;
  FILE *temp_file = tmpfile();

  cfile = fopen(object_path, "rb");

  if (cfile == NULL) {
    fprintf((stderr), "File not found");
    return;
  }

  if (temp_file == NULL) {
    fprintf((stderr), "Error creating file");
    return;
  }

  inf(cfile, temp_file);
  rewind(temp_file);

  char buffer[256];

  // fgets(buffer, sizeof(buffer), temp_file);

  // char *delimPtr = (char *)memchr(buffer, ' ', 12);
  // delimPtr[0] = '\0';

  // printf("%s\n", buffer);
  // printf("%s\n", delimPtr + 1);

  // char *fmt = buffer;
  // char *size_string = delimPtr;

  // printf("%s", delimPtr);

  // fgets(buffer + 1, sizeof(buffer), temp_file);
  // printf("%s", buffer);

  // while (fgets(buffer, sizeof(buffer), temp_file) != NULL) {
  //   printf("%s", buffer);
  // }

  do {
    char c = fgetc(temp_file);

    if (c == '\0') {
      break;
    }
  } while (1);

  do {
    char c = fgetc(temp_file);

    if (c == EOF) {
      break;
    }

    printf("%c", c);
  } while (1);

  fclose(cfile);
  fclose(temp_file);
}