#include "lib/zpipe.h"
#include "objects.h"
#include <dirent.h>
#include <errno.h>
#include <openssl/sha.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <zlib.h>

#include "defs.h"

void get_file_path_from_sha(char *object_path, char *object_sha) {
  sprintf(object_path, ".git/objects/%c%c/%s", object_sha[0], object_sha[1],
          object_sha + 2);
}

void hash_file(char hex[41], char *to_hash, size_t size) {
  unsigned char hash[SHA_DIGEST_LENGTH]; // == 20

  SHA1((unsigned char *)to_hash, size, hash);

  for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
    sprintf(hex + (i * 2), "%02x", hash[i]);
  }
}

FILE *read_git_object_from_sha(char *object_sha) {
  FILE *ofile;
  FILE *temp_file = tmpfile();

  if (temp_file == NULL) {
    fprintf((stderr), "Error creating file");
  }

  char path[200];
  get_file_path_from_sha((char *)&path, object_sha);

  ofile = fopen(path, "rb");

  if (ofile == NULL) {
    fprintf((stderr), "File not found");
  }

  inf(ofile, temp_file);
  rewind(temp_file);

  fclose(ofile);

  return temp_file;
};

void write_git_object(GitObject *object, char *path) {
  FILE *ofile = fopen(path, "rb");

  if (ofile == NULL) {
    fprintf((stderr), "File not found");
    return;
  }

  fseek(ofile, 0, SEEK_END);
  long content_size = ftell(ofile);
  rewind(ofile);

  char c;
  int i = 0;
  char contents[content_size];
  while ((c = fgetc(ofile)) != EOF) {
    contents[i] = c;
    contents[i + 1] = '\0';
    i++;
  }

  int header_size = snprintf(NULL, 0, "blob %ld", content_size);
  int file_size = header_size + 1 + content_size;

  char to_hash[file_size];
  snprintf(to_hash, header_size + 1, "blob %ld", content_size);
  memcpy(to_hash + header_size + 1, contents, sizeof(contents));

  char sha1_hex[41];
  hash_file(sha1_hex, to_hash, file_size);

  char object_path[56];
  get_file_path_from_sha(object_path, sha1_hex);

  char folder_path[16];
  snprintf(folder_path, sizeof(folder_path), "%s", object_path);

  if (mkdir(folder_path, 0777) != 0) {
    // printf("Didnt create folder");
  };

  FILE *temp_file = tmpfile();
  FILE *result_file = fopen(object_path, "wb");

  if (result_file != NULL) {
    fwrite(to_hash, sizeof(char), file_size, temp_file);
    rewind(temp_file);

    def(temp_file, result_file, Z_DEFAULT_COMPRESSION);
  }

  object->sha1 = strdup(sha1_hex);

  fclose(ofile);
  fclose(temp_file);

  if (result_file != NULL)
    fclose(result_file);
}

int compare_object_names(const void *a, const void *b) {
  const GitObject *obj_1 = (GitObject *)a;
  const GitObject *obj_2 = (GitObject *)b;
  return strcmp(obj_1->file_name, obj_2->file_name);
}

void print_git_tree(GitObjectArray *objects) {
  qsort(objects->data, objects->length, sizeof(GitObject),
        compare_object_names);

  for (int i = 0; i < objects->length; i++) {
    printf("%s %s\n", objects->data[i].file_name, objects->data[i].sha1);
  }
}

void read_tree(char *path) {
  DIR *rootdir;
  struct dirent *entry;

  rootdir = opendir(path);

  GitObjectArray objects = {};

  char *ptrPath = strdup(path);
  char *nextPath = {};

  size_t pathSize = strlen(path);
  size_t nextPathSize = 0;

  while ((entry = readdir(rootdir)) != NULL) {
    if ((strncmp(entry->d_name, "..", 2) == 0) ||
        ((strncmp(entry->d_name, ".", 1) == 0) &&
         (strlen(entry->d_name) == 1)) ||
        ((strncmp(entry->d_name, ".git", 4) == 0) &&
         (strlen(entry->d_name) == 4))) {
      continue;
    }

    nextPathSize = (strlen(entry->d_name) + pathSize) + 10;
    if (strlen(ptrPath) < nextPathSize) {
      nextPath = realloc(ptrPath, nextPathSize);
    }
    strcat(nextPath, "/");
    strcat(nextPath, entry->d_name);

    // check file
    if (entry->d_type == DT_REG) {
      GitObject object = {
          .type = BLOB,
          .file_name = entry->d_name,
          .sha1 = "",
      };

      write_git_object(&object, nextPath);

      arr_append(objects, object);
    }

    // check tree
    if (entry->d_type == DT_DIR) {
      // read_tree(nextPath);

      GitObject object = {
          .type = TREE,
          .file_name = entry->d_name,
      };

      arr_append(objects, object);
    }

    ptrPath = strdup(path);
  }

  print_git_tree(&objects);

  char tree_sha[41];
}
