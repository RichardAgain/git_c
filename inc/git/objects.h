#include <stddef.h>
#include <stdio.h>

typedef struct {
  enum {
    BLOB,
    TREE,
    COMMIT,
    TAG,
  } type;
  unsigned int file_mode;
  char *file_name;
  unsigned char *sha1_raw;
  char *sha1_hex;
} GitObject;

typedef struct {
  GitObject *data;
  size_t length;
  size_t capacity;
} GitObjectArray;

unsigned char *read_tree(char *path);

void get_file_path_from_sha(char *object_path, char *object_sha);
FILE *read_git_object_from_sha(char *object_sha);
