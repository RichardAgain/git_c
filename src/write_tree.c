#include "objects.h"
#include <dirent.h>

typedef struct dirent dirent;

void write_tree(int argc, char **argv) {
  //   DIR *rootdir;
  //   dirent *entry;

  GitObject *object = {};

  read_tree(".");

  //   rootdir = opendir(".");

  //   while ((entry = readdir(rootdir)) != NULL) {
  //     if (entry->d_type == DT_REG) {
  //     }

  //     if (entry->d_type == DT_DIR) {
  //     }

  //     printf("%s\n", entry->d_name);
  //   }

  return;
}