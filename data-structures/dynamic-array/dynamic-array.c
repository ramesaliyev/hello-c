#include "dynamic-array.h"

Array * create_array(size_t size) {
  Array * array = (Array *) malloc(sizeof(Array));
  array->array = (int *) calloc(size, sizeof(int));
  array->used = 0;
  array->size = size;

  return array;
}

Array * create_array_from_array(int * arr, size_t size) {
  Array * array = create_array(size);
  array->used = size;

  memcpy(array->array, arr, size * sizeof(int));
  return array;
}

void insert_array(Array * array, int element) {
  if (array->used == array->size) {
    array->size *= 2;
    array->array = (int *) realloc(array->array, array->size * sizeof(int));
  }
  array->array[array->used++] = element;
}

int get_array(Array * array, int index) {
  if (index > array->size) {
    return 0;
  }

  return array->array[index];
}

void swap_array(Array * array, int index_a, int index_b) {
  int tmp = array->array[index_a];
  array->array[index_a] = array->array[index_b];
  array->array[index_b] = tmp;
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
