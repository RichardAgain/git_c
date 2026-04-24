#include "git/objects.h"
#include "git/zpipe.h"
#include <dirent.h>
#include <openssl/sha.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <zconf.h>
#include <zlib.h>

#include "git/defs.h"
#include "git/objects.h"
#include "git/zpipe.h"

void get_file_path_from_sha(char *object_path, char *object_sha) {
  sprintf(object_path, ".git/objects/%c%c/%s", object_sha[0], object_sha[1],
          object_sha + 2);
}

void sha_to_hex(char hex[41], unsigned char *hash) {
  for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
    sprintf(hex + (i * 2), "%02x", hash[i]);
  }
}

int read_object_header(GitObject *object, FILE *file) {
  char c;
  int i = 0;
  char header_s[MAX_HEADER_SIZE] = {0};
  while ((c = fgetc(file)) != '\0' && c != EOF && i < sizeof(header_s) - 1) {
    header_s[i++] = c;
  }

  object_header_t header = {0};
  header.size = 0;
  char type_s[7];

  sscanf(header_s, "%s %zu", type_s, &header.size);
  strcpy(header.type_s, type_s);

  object->header = header;

  return i;
}

int read_object_contents(GitObject *object, FILE *file, size_t size) {
  char c;
  int i = 0;
  unsigned char *contents = calloc(size, sizeof(unsigned char));
  if (contents == NULL) {
    return -1;
  }

  fread(contents, sizeof(unsigned char), size, file);

  object->contents = contents;

  return i;
}

GitObject *parse_git_object(FILE *file, size_t total_size) {
  GitObject *git_object = malloc(sizeof(GitObject));
  git_object->contents = NULL;

  int header_size = read_object_header(git_object, file);

  if (read_object_contents(git_object, file, total_size - header_size) < 0) {
    perror("git contents");
    free(git_object);
    return NULL;
  };

  return git_object;
};

git_tree_t *parse_git_tree(GitObject *tree_data) {
  if (tree_data == NULL) {
    return NULL;
  }

  if (strcmp(tree_data->header.type_s, "tree") != 0) {
    printf("Not a tree\n");
    return NULL;
  }

  git_tree_t *git_tree = calloc(1, sizeof(git_tree_t));
  if (!git_tree) {
    perror("parse-tree");
    return NULL;
  }
  tree_entry_t *ptemp = NULL;

  char filename[FILENAME_MAX];
  char mode[7];
  char sha[20];
  char hex[41];

  int idx = 0;
  int mode_idx = 0;
  int filename_idx = 0;

  int count = 0;

  while (idx < tree_data->header.size) {
    char c;
    memset(filename, 0, FILENAME_MAX);
    memset(mode, 0, sizeof(mode));

    while ((c = tree_data->contents[idx]) != ' ' && c != '\0' && c != EOF) {
      mode[mode_idx++] = c;
      idx++;
    }
    idx++;

    while ((c = tree_data->contents[idx]) != '\0' && c != EOF) {
      filename[filename_idx++] = c;
      idx++;
    }
    idx++;

    memcpy(sha, tree_data->contents + idx, 20);
    idx += 20;

    sha_to_hex(hex, (unsigned char *)sha);

    tree_entry_t entry = {0};

    entry.name = strdup(filename);
    strcpy(entry.mode, mode);
    memcpy(entry.sha, sha, 20);
    strcpy(entry.hex, hex);

    ptemp = realloc(git_tree->entries, (count + 1) * sizeof(tree_entry_t));
    if (ptemp == NULL) {
      perror("realloc");
      // free tree method
      return NULL;
    }
    git_tree->entries = ptemp;
    git_tree->entries[count] = entry;

    count++;
    mode_idx = 0;
    filename_idx = 0;
  }

  git_tree->length = count;

  return git_tree;
}

void writeGitObjectFromSha(unsigned char *file_contents, char *sha1_hex,
                           size_t file_size) {
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
    fwrite(file_contents, sizeof(char), file_size, temp_file);
    rewind(temp_file);

    def(temp_file, result_file, Z_DEFAULT_COMPRESSION);
  }

  fclose(temp_file);

  if (result_file != NULL)
    fclose(result_file);
}

void write_git_object(GitObject *object, char *path) {
  FILE *ofile = fopen(path, "rb");

  if (ofile == NULL) {
    perror("File not found");
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

  unsigned char to_hash[file_size];
  snprintf((char *)to_hash, header_size + 1, "blob %ld", content_size);
  memcpy(to_hash + header_size + 1, contents, sizeof(contents));

  unsigned char hash[SHA_DIGEST_LENGTH]; // == 20
  SHA1((unsigned char *)to_hash, file_size, hash);

  char sha1_hex[41];
  sha_to_hex(sha1_hex, hash);

  writeGitObjectFromSha(to_hash, sha1_hex, file_size);

  object->sha1_raw = malloc(20);
  memcpy(object->sha1_raw, hash, 20);

  object->sha1_hex = strdup(sha1_hex);

  fclose(ofile);
}

char *getGitTypeName(GitObject *object) {
  switch (object->type) {
  case TREE:
    return "tree";
  case BLOB:
    return "blob";
  default:
    return "blob";
  }
}

int compare_object_names(const void *a, const void *b) {
  const GitObject *obj_1 = (GitObject *)a;
  const GitObject *obj_2 = (GitObject *)b;
  return strcmp(obj_1->file_name, obj_2->file_name);
}

void print_git_tree(GitObjectArray *objects) {
  for (int i = 0; i < objects->length; i++) {
    printf("%s %s\t%s\n", getGitTypeName(&objects->data[i]),
           objects->data[i].sha1_hex, objects->data[i].file_name);
  }
}

unsigned char *read_tree(char *path) {
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

    struct stat fileStat;

    // check file
    if (entry->d_type == DT_REG) {
      stat(nextPath, &fileStat);

      GitObject object = {
          .type = BLOB,
          .file_name = entry->d_name,
          .file_mode = fileStat.st_mode,
          .sha1_raw = 0,
          .sha1_hex = 0,
      };

      write_git_object(&object, nextPath);

      arr_append(objects, object);
    }

    // check tree
    if (entry->d_type == DT_DIR) {
      stat(nextPath, &fileStat);

      GitObject object = {
          .type = TREE,
          .file_name = entry->d_name,
          .file_mode = fileStat.st_mode,
          .sha1_raw = read_tree(nextPath),
      };

      arr_append(objects, object);
    }

    ptrPath = strdup(path);
  }

  qsort(objects.data, objects.length, sizeof(GitObject), compare_object_names);

  unsigned char tree_contents[4098];
  size_t contents_position = 0;

  memset(tree_contents, 0, 4098);

  for (int i = 0; i < objects.length; i++) {
    GitObject object = objects.data[i];

    if (sizeof(object) == 0) {
      continue;
    }

    char object_header[100];
    size_t object_header_size = snprintf(object_header, 100, "%o %s",
                                         object.file_mode, object.file_name);

    for (size_t j = 0; j < object_header_size; j++) {
      tree_contents[contents_position++] = object_header[j];
    }

    tree_contents[contents_position++] = '\0';

    unsigned char sha1_raw[25];
    memset(sha1_raw, 0, 25);
    memcpy(sha1_raw, object.sha1_raw, 25);

    for (size_t j = 0; j < 20; j++) {
      tree_contents[contents_position++] = object.sha1_raw[j];
    }
  }

  char tree_header[100];
  memset(tree_header, 0, 100);
  size_t header_size =
      snprintf(tree_header, 100, "tree %ld", contents_position - 1);
  size_t file_size = header_size + contents_position;

  // printf("header: %s size: %zu\n", tree_header, header_size);

  unsigned char to_hash[file_size];
  memset(to_hash, 0, file_size);
  memcpy(to_hash, tree_header, header_size);
  memset(to_hash + header_size, 0, 1);
  memcpy(to_hash + header_size + 1, tree_contents, file_size + 1);

  // for (size_t i = 0; i < file_size; i++) {
  //   printf("%c", to_hash[i]);
  // }

  unsigned char *hash = malloc(SHA_DIGEST_LENGTH); // == 20
  SHA1((unsigned char *)to_hash, file_size, hash);

  char tree_sha_hex[41];
  sha_to_hex(tree_sha_hex, hash);
  // sha_to_hex(tree_sha_hex, objects.data[0].sha1_raw);

  // printf("\n%s\n", tree_sha_hex);

  writeGitObjectFromSha(to_hash, tree_sha_hex, file_size);

  free(objects.data);
  closedir(rootdir);

  return hash;
}
