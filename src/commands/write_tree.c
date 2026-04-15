#include "git/objects.h"
#include <dirent.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct dirent dirent;

void write_tree(int argc, char **argv) {
  //   DIR *rootdir;
  //   dirent *entry;

  unsigned char *tree_sha = read_tree(".");

  if (tree_sha == NULL) {
    fprintf(stderr, "Failed to read tree\n");
    return;
  }

  char tree_sha_hex[41];
  for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
    sprintf(tree_sha_hex + (i * 2), "%02x", (unsigned char)tree_sha[i]);
  }

  printf("%s\n", tree_sha_hex);

  free(tree_sha);

  return;
}
