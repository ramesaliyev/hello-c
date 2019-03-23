#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../dynamic-array/dynamic-array.h"

#ifndef HEAP /* Include guard */
#define HEAP

typedef struct {
  Array * data;
} Heap;

Heap * create_heap();

Heap * create_heap_from_array(int * arr, size_t length);

int heap_get_size(Heap * heap);

int heap_get_length(Heap * heap);

int heap_pop(Heap * heap);

void heap_push(Heap * heap, int value);

int heap_get(Heap * heap, int index);

void heap_set(Heap * heap, int index, int value);

void heap_swap(Heap * heap, int index_a, int index_b);

void print_heap(Heap * heap);

#endif