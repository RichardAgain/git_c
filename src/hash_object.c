
#include "lib/zpipe.h"
#include <openssl/sha.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <zlib.h>

void hash_object(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(stderr, "Usage: ./your_program.sh hash-object [<args>] <object>\n");
    return;
  }

  const char *param = argv[2];
  const char *object_path = argv[3];

  if (strcmp(argv[2], "-w") != 0) {
    fprintf((stderr), "Argument not implemented");
    return;
  }

  FILE *ofile = fopen(object_path, "rb");

  if (ofile == NULL) {
    fprintf((stderr), "File not found");
    return;
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
    return;
  }

  fwrite(to_hash, sizeof(char), file_size, temp_file);
  rewind(temp_file);

  def(temp_file, result_file, Z_DEFAULT_COMPRESSION);

  fclose(ofile);
  fclose(temp_file);
  fclose(result_file);
}
