#include <dirent.h>
#include <openssl/sha.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "git/defs.h"
#include "git/files.h"
#include "git/objects.h"

typedef struct dirent dirent;

char *combine_path(const char *dir, const char *file) {
  size_t len = strlen(dir) + strlen(file) + 2;
  char *full_path = malloc(len);

  if (full_path == NULL) return NULL;

  snprintf(full_path, len, "%s/%s", dir, file);
  return full_path;
}

void git_tree_append(git_tree_t *tree, tree_entry_t entry) {
  tree->entries =
      realloc(tree->entries, (tree->length + 1) * sizeof(tree_entry_t));
  tree->entries[tree->length++] = entry;
}

git_tree_t *iterate_tree(char *tree_path) {
  struct dirent *entry;
  DIR *dir = opendir(tree_path);

  if (dir == NULL) {
    return NULL;
  }

  tree_entry_t *ptemp = NULL;
  git_tree_t *tree = malloc(sizeof(git_tree_t));
  tree->data = NULL;
  tree->entries = NULL;
  tree->length = 0;
  tree->capacity = 0;

  while ((entry = readdir(dir)) != NULL) {
    if ((strncmp(entry->d_name, "..", 2) == 0) ||
        ((strncmp(entry->d_name, ".", 1) == 0) &&
         (strlen(entry->d_name) == 1)) ||
        ((strncmp(entry->d_name, ".git", 4) == 0) &&
         (strlen(entry->d_name) == 4))) {
      continue;
    }

    struct stat file_stat = {0};

    char *file_path = combine_path(tree_path, entry->d_name);
    if (file_path == NULL) {
      return NULL;
    }

    if (entry->d_type == DT_REG) {
      file_result_t fr = read_file_from_path(file_path);

      unsigned char *contents = calloc(fr.size, sizeof(unsigned char));
      if (contents == NULL) return NULL;

      fread(contents, sizeof(unsigned char), fr.size, fr.file);

      FILE *temp = tmpfile();
      if (temp == NULL) {
        perror("tmpfile");
        return NULL;
      }

      GitObject *object = write_git_object(temp, contents, fr.size);

      tree_entry_t tree_entry = {0};

      tree_entry.name = entry->d_name;
      strncpy(tree_entry.mode, "000000", 6);
      strncpy(tree_entry.hex, object->sha1_hex, 41);
      memcpy(tree_entry.sha, object->sha1_raw, 20);

      git_tree_append(tree, tree_entry);
    }

    if (entry->d_type == DT_DIR) {
      git_tree_t *child_tree = iterate_tree(file_path);

      tree_entry_t tree_entry = {0};

      tree_entry.name = entry->d_name;
      strncpy(tree_entry.mode, "040000", 6);
      strncpy(tree_entry.hex, child_tree->data->sha1_hex, 41);
      memcpy(tree_entry.sha, child_tree->data->sha1_raw, 20);

      git_tree_append(tree, tree_entry);
    }

    if (entry->d_type == DT_UNKNOWN) {
      printf("File type not supported or unkwown\n");
      return NULL;
    }

    free(file_path);
  }

  // qsort(objects.data, objects.length, sizeof(GitObject), compare_object_names);

  FILE *tree_f = tmpfile();
  size_t contents_size = 0;
  unsigned char contents[4096];

  for (int i = 0; i < tree->length; i++) {
      size_t header_size = strlen(tree->entries[i].name) + 8;
      size_t total_size = header_size + 20 + 1;

      char entry_header[header_size];
      snprintf(entry_header, header_size, "%s %s", tree->entries[i].mode, tree->entries[i].name);

      memcpy(contents + contents_size, entry_header, header_size);
      memset(contents + contents_size + header_size, 0, 1);
      memcpy(contents + contents_size + header_size + 1, tree->entries[i].sha, 20);

      contents_size += total_size;
  }

  tree->data = write_git_object(tree_f, contents, contents_size);
  if (tree->data == NULL) {
      return NULL;
  }

  // FILE *test = fopen("./test.txt", "w+");
  // fwrite(contents, sizeof(unsigned char), contents_size, test);
  // fclose(test);

  closedir(dir);
  return tree;
}

void write_tree(int argc, char **argv) {
  git_tree_t *tree = iterate_tree(".");

  if (tree == NULL) {
      printf("Error writing tree\n");
      return;
  }

  printf("%s\n", tree->data->sha1_hex);

  free(tree);

  return;
}
