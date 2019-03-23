#include "max-heap.h"

// Max Heap:
// In this type of heap, the value of parent node will always be greater
// than or equal to the value of child node across the tree and the node
// with highest value will be the root node of the tree.

void max_heapify(Heap * heap, int index, size_t length) {
  int index_left = 2 * index;
  int index_right = 2 * index + 1;
  int index_largest = index;

  if (index_left <= length && heap_get(heap, index_left) > heap_get(heap, index)) {
    index_largest = index_left;
  }

  if (index_right <= length && heap_get(heap, index_right) > heap_get(heap, index_largest)) {
    index_largest = index_right;
  }

  if (index_largest != index) {
    heap_swap(heap, index, index_largest);
    max_heapify(heap, index_largest, length);
  }
}

Heap * build_max_heap(Heap * heap, int length) {
  // Elements from heap[length/2 + 1] are leaf nodes.
  // So we dont need to process them.
  // We can use max_heapify function in a bottom up manner
  // on remaining nodes, so that we can cover each node of tree.
  for (int i = (length/2); i >= 1; i--) {
    max_heapify(heap, i, length);
  }

  return heap;
}