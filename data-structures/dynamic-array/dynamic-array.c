#include "dynamic-array.h"

Array * create_array(size_t size) {
  Array * array = (Array *) malloc(sizeof(Array));
  array->array = (int *) calloc(size, sizeof(int));
  array->used = 0;
  array->size = size;

  return array;
}

void insert_array(Array * array, int element) {
  if (array->used == array->size) {
    array->size *= 2;
    array->array = (int *) realloc(array->array, array->size * sizeof(int));
  }
  array->array[array->used++] = element;
}

void free_array(Array * array) {
  free(array->array);
  array->array = NULL;
  array->used = array->size = 0;
}

void print_array(Array * array) {
  int i;
  printf("Array, used/size %zu/%zu: Items: {", array->used, array->size);
  for (i = 0; i < array->used; i++) {
    printf("%d", array->array[i]);
    if (i != array->used - 1) printf(", ");
  }
  printf("}\n");
}
