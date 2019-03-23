#include "priority-queue.h"

// Priority Queue is similar to queue where we insert an element
// from the back and remove an element from front, but with a
// difference that the logical order of elements in the priority
// queue depends on the priority of the elements. The element with
// highest priority will be moved to the front of the queue and
// one with lowest priority will move to the back of the queue.
// Thus it is possible that when you enqueue an element at the
// back in the queue, it can move to front because of its highest
// priority.

// We can think of many ways to implement the priority queue.

// Naive Approach:
// Suppose we have N elements and we have to insert these elements
// in the priority queue. We can use list and can insert elements
// in O(N) time and can sort them to maintain a priority queue in
// O(NlogN) time.

// Efficient Approach:
// We can use heaps to implement the priority queue. It will take
// O(logN) time to insert and delete each element in the priority queue.

// Based on heap structure, priority queue also has two types
// max-priority queue and min-priority queue.

Heap * create_priority_queue() {
  return create_heap();
}

int priority_queue_get_max(Heap * heap) {
  return heap_get(heap, 1);
}

int priority_queue_extract_max(Heap * heap) {
  int heap_length = heap_get_length(heap);

  if (heap_length == 0) {
    return 0;
  }

  int max = heap_get(heap, 1);

  heap_swap(heap, 1, heap_length);
  heap_pop(heap);
  max_heapify(heap, 1, heap_length - 1);

  return max;
}

void priority_queue_increase(Heap * heap, int index, int value) {
  if (value >= heap_get(heap, index)) {
    heap_set(heap, index, value);

    while (index > 1 && heap_get(heap, index / 2) < heap_get(heap, index)) {
      heap_swap(heap, index / 2, index);
      index /= 2;
    }
  }
}

void priority_queue_insert(Heap * heap, int value) {
  heap_push(heap, -999999999);
  priority_queue_increase(heap, heap_get_length(heap), value);
}