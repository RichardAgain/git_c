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

GitObject *write_git_object(FILE *out, void *contents, size_t contents_size) {
  GitObject *object = malloc(sizeof(GitObject));

  char header_s[MAX_HEADER_SIZE];
  int header_size =
      snprintf(header_s, MAX_HEADER_SIZE, "blob %lu", contents_size);

  fwrite(header_s, sizeof(char), header_size, out);
  fseek(out, 1, SEEK_CUR);
  fwrite(contents, sizeof(char), contents_size, out);

  size_t file_size = header_size + contents_size + 1;
  unsigned char to_hash[file_size];

  fseek(out, 0, SEEK_SET);
  fread(to_hash, sizeof(unsigned char), file_size, out);

  unsigned char hash[SHA_DIGEST_LENGTH];
  SHA1((unsigned char *)to_hash, file_size, hash);

  char hex[41];
  sha_to_hex(hex, hash);

  memcpy(object->sha1_raw, hash, 20);
  memcpy(object->sha1_hex, hex, 41);

  return object;
}

// int compare_object_names(const void *a, const void *b) {
//   const GitObject *obj_1 = (GitObject *)a;
//   const GitObject *obj_2 = (GitObject *)b;
//   return strcmp(obj_1->file_name, obj_2->file_name);
// }
