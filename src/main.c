#include <assert.h>
#include <errno.h>
#include <openssl/sha.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <zconf.h>
#include <zlib.h>

#include "git/commands.h"

int main(int argc, char *argv[]) {
  // Disable output buffering
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);

  if (argc < 2) {
    fprintf(stderr, "Usage: ./your_program.sh <command> [<args>]\n");
    return 1;
  }

  const char *command = argv[1];

  if (strcmp(command, "init") == 0) {
    // You can use print statements as follows for debugging, they'll be visible
    // when running tests.
    fprintf(stderr, "Logs from your program will appear here!\n");

    // TODO: Uncomment the code below to pass the first stage
    if (mkdir(".git", 0755) == -1 || mkdir(".git/objects", 0755) == -1 ||
        mkdir(".git/refs", 0755) == -1) {
      fprintf(stderr, "Failed to create directories: %s\n", strerror(errno));
      return 1;
    }

    FILE *headFile = fopen(".git/HEAD", "w");
    if (headFile == NULL) {
      fprintf(stderr, "Failed to create .git/HEAD file: %s\n", strerror(errno));
      return 1;
    }
    fprintf(headFile, "ref: refs/heads/main\n");
    fclose(headFile);

    printf("Initialized git directory\n");
  }

  else if (strncmp(command, "cat-file", 8) == 0) {
    cat_file(argc, argv);
  }

  else if (strncmp(command, "hash-object", 11) == 0) {
    hash_object(argc, argv);
  }

  else if (strncmp(command, "ls-tree", 7) == 0) {
    ls_tree(argc, argv);
  }

  else if (strncmp(command, "write-tree", 10) == 0) {
    write_tree(argc, argv);
  }

  else {
    fprintf(stderr, "Unknown command %s\n", command);
    return 1;
  }

  return 0;
}
