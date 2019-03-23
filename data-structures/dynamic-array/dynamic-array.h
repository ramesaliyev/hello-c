#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int *array;
  size_t used;
  size_t size;
} Array;

Array * create_array(size_t size);

void insert_array(Array * array, int element);

void free_array(Array * array);

void print_array(Array * array);
