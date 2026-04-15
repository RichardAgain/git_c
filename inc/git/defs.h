
#define arr_append(array, elem)                                                \
  do {                                                                         \
    array.data = realloc(array.data, (array.length + 1) * sizeof(elem));       \
    array.data[array.length++] = elem;                                         \
  } while (0);
