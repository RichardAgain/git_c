#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <zconf.h>

#include "zpipe.c"

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
  } else if (strncmp(command, "cat-file", 8) == 0) {
    if (argc < 4) {
      fprintf(stderr, "Usage: ./your_program.sh cat-file [<args>] <object>\n");
      return 1;
    }

    const char *param = argv[2];
    const char *object_name = argv[3];

    if (strcmp(argv[2], "-p") != 0) {
      fprintf((stderr), "Argument not implemented");
      return 1;
    }

    char object_path[200];
    sprintf(object_path, ".git/objects/%c%c/%s", object_name[0], object_name[1],
            object_name + 2);

    FILE *cfile;
    FILE *temp_file = tmpfile();

    cfile = fopen(object_path, "r");

    if (cfile == NULL) {
      fprintf((stderr), "File not found");
      return 1;
    }

    if (temp_file == NULL) {
      fprintf((stderr), "Error creating file");
      return 1;
    }

    inf(cfile, temp_file);
    rewind(temp_file);

    char buffer[100];

    fgets(buffer, sizeof(buffer), temp_file);

    char *fmt = strtok(buffer, " ");
    // printf("%s\n", fmt);

    char *size_string = strtok(NULL, "");
    // printf("%s\n", size_string);

    while (fgets(buffer, sizeof(buffer), temp_file) != NULL) {
      printf("%s", buffer);
    }

    fclose(cfile);
    fclose(temp_file);

  } else {
    fprintf(stderr, "Unknown command %s\n", command);
    return 1;
  }

  return 0;
}
