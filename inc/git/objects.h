#include <stddef.h>
#include <stdio.h>

#define MAX_HEADER_SIZE 64

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
  char sha1_hex[41];
  unsigned char sha1_raw[20];
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

GitObject *parse_git_object(FILE *file, size_t total_size);
git_tree_t *parse_git_tree(GitObject *data);

GitObject *write_git_object(void *contents, size_t size, FILE *file);

unsigned char *read_tree(char *path);