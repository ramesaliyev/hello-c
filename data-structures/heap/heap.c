#include "heap.h"

// https://www.hackerearth.com/practice/data-structures/trees/heapspriority-queues/tutorial/

// A heap is a tree-based data structure in which all the nodes of the tree are in a specific order.

// For example, if X is the parent node of Y, then the value of X follows a specific order with
// respect to the value of Y and the same order will be followed across the tree.

// The maximum number of children of a node in a heap depends on the type of heap.
// However, in the more commonly-used heap type, there are at most 2 children of
// a node and it's known as a Binary heap.

// In binary heap, if the heap is a complete binary tree with N nodes,
// then it has smallest possible height which is  log2(N).

Heap * create_heap() {
  Heap * heap = (Heap *) malloc(sizeof(Heap));
  heap->data = create_array(10);

  return heap;
}

Heap * create_heap_from_array(int * arr, size_t length) {
  Heap * heap = (Heap *) malloc(sizeof(Heap));
  heap->data = create_array_from_array(arr, length);

  return heap;
}

int heap_get_size(Heap * heap) {
  return heap->data->size;
}

int heap_get_length(Heap * heap) {
  return heap->data->used;
}

int heap_pop(Heap * heap) {
  return pop_array(heap->data);
}

void heap_push(Heap * heap, int value) {
  insert_array(heap->data, value);
}

int heap_get(Heap * heap, int index) {
  return get_array(heap->data, index - 1);
}

void heap_set(Heap * heap, int index, int value) {
  set_array(heap->data, index - 1, value);
}

void heap_swap(Heap * heap, int index_a, int index_b) {
  swap_array(heap->data, index_a - 1, index_b - 1);
}

void print_heap(Heap * heap) {
  printf("Heap ");
  print_array(heap->data);
}