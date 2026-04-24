#include <stddef.h>
#include <stdio.h>

#define MAX_HEADER_SIZE 64
#define OBJECT_PATH_LENGTH 56

typedef struct {
  char type_s[7];
  size_t size;
} object_header_t;

typedef struct {
  object_header_t header;
  unsigned char *contents;

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
  char *name;
  char type[7];
  char mode[7];
  unsigned char sha[20];
  char hex[41];
} tree_entry_t;

typedef struct {
  GitObject *data;
  tree_entry_t *entries;
  size_t length;
  size_t capacity;
} git_tree_t;

typedef struct {
  GitObject *data;
  size_t length;
  size_t capacity;
} GitObjectArray;

void get_file_path_from_sha(char *object_path, char *object_sha);
GitObject *read_git_object_from_sha(char *object_sha);
git_tree_t *parse_git_tree(GitObject *data);

unsigned char *read_tree(char *path);