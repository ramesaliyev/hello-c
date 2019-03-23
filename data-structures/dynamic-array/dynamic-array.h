#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef DYNAMIC_ARRAY /* Include guard */
#define DYNAMIC_ARRAY

typedef struct {
  int *array;
  size_t used;
  size_t size;
} Array;

Array * create_array(size_t size);

Array * create_array_from_array(int * array, size_t size);

void insert_array(Array * array, int element);

int get_array(Array * array, int index);

void swap_array(Array * array, int index_a, int index_b);

void free_array(Array * array);

void print_array(Array * array);

#endif