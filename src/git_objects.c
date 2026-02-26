#include "lib/zpipe.h"
#include <stdio.h>

// typedef struct {
//   char object_sha[FILENAME_MAX];
//   char object_mode[7];
//   char sha1[41];
// } GitObject;

void get_file_path_from_sha(char *object_path, char *object_sha) {
  sprintf(object_path, ".git/objects/%c%c/%s", object_sha[0], object_sha[1],
          object_sha + 2);
}

FILE *read_git_object_from_sha(char *object_sha) {
  FILE *ofile;
  FILE *temp_file = tmpfile();

  if (temp_file == NULL) {
    fprintf((stderr), "Error creating file");
  }

  char path[200];
  get_file_path_from_sha(path, object_sha);

  ofile = fopen(path, "rb");

  if (ofile == NULL) {
    fprintf((stderr), "File not found");
  }

  inf(ofile, temp_file);
  rewind(temp_file);

  fclose(ofile);

  return temp_file;
}
