#include "heap.h"

#ifndef MAX_HEAP /* Include guard */
#define MAX_HEAP

void max_heapify(Heap * heap, int index, size_t length);

Heap * build_max_heap(Heap * heap, int length);

#endif