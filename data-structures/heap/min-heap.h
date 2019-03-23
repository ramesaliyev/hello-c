#include "heap.h"

#ifndef MIN_HEAP /* Include guard */
#define MIN_HEAP

void min_heapify(Heap * heap, int index, size_t length);

Heap * build_min_heap(Heap * heap, int length);

#endif