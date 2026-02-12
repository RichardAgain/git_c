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

#include "lib/zpipe.h"

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

    cfile = fopen(object_path, "rb");

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

  else if (strncmp(command, "hash-object", 11) == 0) {
    if (argc < 3) {
      fprintf(stderr,
              "Usage: ./your_program.sh hash-object [<args>] <object>\n");
      return 1;
    }

    const char *param = argv[2];
    const char *object_path = argv[3];

    if (strcmp(argv[2], "-w") != 0) {
      fprintf((stderr), "Argument not implemented");
      return 1;
    }

    FILE *ofile = fopen(object_path, "rb");

    if (ofile == NULL) {
      fprintf((stderr), "File not found");
      return 1;
    }

    fseek(ofile, 0, SEEK_END);
    long content_size = ftell(ofile);
    rewind(ofile);

    // fprintf(temp_file, "blob %ld", header_size);
    // fputc('\0', temp_file);

    char c;
    int i = 0;
    char contents[content_size];
    while ((c = fgetc(ofile)) != EOF) {
      contents[i] = c;
      contents[i + 1] = '\0';
      i++;
    }

    int header_size = snprintf(NULL, 0, "blob %ld", content_size);
    // int header_size = 7;

    int file_size = header_size + 1 + content_size;
    // printf("%d", file_size);

    char to_hash[file_size];
    snprintf(to_hash, header_size + 1, "blob %ld", content_size);
    memcpy(to_hash + header_size + 1, contents, sizeof(contents));

    // printf("%s\n", to_hash);

    // char data[] = "Hello, world!";
    // size_t length = strlen(data);

    // unsigned char hash[SHA_DIGEST_LENGTH];
    // SHA1(data, length, hash);

    // const unsigned char str[] = "Original String";
    unsigned char hash[SHA_DIGEST_LENGTH]; // == 20

    SHA1((unsigned char *)to_hash, file_size, hash);
    // SHA1((unsigned char *)to_hash, sizeof(to_hash) - 1, hash);
    // printf("%s\n", hash);

    char sha1_hex[40];
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
      sprintf(sha1_hex + (i * 2), "%02x", hash[i]);
    }
    printf("%s\n", sha1_hex);

    char file_path[56];
    sprintf(file_path, ".git/objects/%c%c/%s", sha1_hex[0], sha1_hex[1],
            sha1_hex + 2);

    char folder_path[16];
    snprintf(folder_path, sizeof(folder_path), "%s", file_path);

    // printf("%s", folder_path);

    if (mkdir(folder_path, 0777) != 0) {
      // printf("Didnt create folder");
    };

    FILE *temp_file = tmpfile();
    FILE *result_file = fopen(file_path, "wb");

    if (result_file == NULL || temp_file == NULL) {
      printf("Error creating file(s)");
      return 1;
    }

    fwrite(to_hash, sizeof(char), file_size, temp_file);
    rewind(temp_file);

    def(temp_file, result_file, Z_DEFAULT_COMPRESSION);

    fclose(ofile);
    fclose(temp_file);
    fclose(result_file);
  }

  else {
    fprintf(stderr, "Unknown command %s\n", command);
    return 1;
  }

  return 0;
}
