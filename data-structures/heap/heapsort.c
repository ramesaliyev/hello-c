#include "heapsort.h"

Heap * heap_sort_ascending(int * array, size_t size) {
  print_array(create_array_from_array(array, size));
  Heap * heap = build_max_heap(create_heap_from_array(array, size), size);
  print_heap(heap);

  for (int i = size; i >= 2; i--) {
    heap_swap(heap, 1, i);
    max_heapify(heap, 1, i - 1);
  }

  return heap;
}

Heap * heap_sort_descending(int * array, size_t size) {
  Heap * heap = build_min_heap(create_heap_from_array(array, size), size);

  for (int i = size; i >= 2; i--) {
    heap_swap(heap, 1, i);
    min_heapify(heap, 1, i - 1);
  }

  return heap;
}