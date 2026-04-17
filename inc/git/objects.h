#include <stddef.h>
#include <stdio.h>

#define MAX_HEADER_SIZE 64
#define OBJECT_PATH_LENGTH 56

typedef struct {
  // enum {
  //   BLOB,
  //   TREE,
  //   COMMIT,
  //   TAG,
  // } type;

  char type_s[7];
  size_t size;
} object_header_t;

typedef struct {
  object_header_t header;
  char *contents;

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
GitObject *read_git_object_from_sha(char *object_sha);
