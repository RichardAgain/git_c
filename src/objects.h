#include <stdio.h>

void get_file_path_from_sha(char *object_path, char *object_sha);
FILE *read_git_object_from_sha(char *object_sha);
