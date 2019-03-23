#include "min-heap.h"

// Min Heap:
// In this type of heap, the value of parent node will always be less than
// or equal to the value of child node across the tree and the node with
// lowest value will be the root node of tree.

void min_heapify(Heap * heap, int index, size_t length) {
  int index_left = 2 * index;
  int index_right = 2 * index + 1;
  int index_smallest = index;

  if (index_left <= length && heap_get(heap, index_left) < heap_get(heap, index)) {
    index_smallest = index_left;
  }

  if (index_right <= length && heap_get(heap, index_right) < heap_get(heap, index_smallest)) {
    index_smallest = index_right;
  }

  if (index_smallest != index) {
    heap_swap(heap, index, index_smallest);
    min_heapify(heap, index_smallest, length);
  }
}

Heap * build_min_heap(Heap * heap, int length) {
  // Elements from heap[length/2 + 1] are leaf nodes.
  // So we dont need to process them.
  // We can use min_heapify function in a bottom up manner
  // on remaining nodes, so that we can cover each node of tree.
  for (int i = (length/2); i >= 1; i--) {
    min_heapify(heap, i, length);
  }

  return heap;
}