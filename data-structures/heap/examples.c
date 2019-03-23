#include "max-heap.h"
#include "min-heap.h"
#include "heapsort.h"
#include "priority-queue.h"

int main(int argc, char **argv) {
  int array[15] = {3,7,5,10,1,9,6,8,4,15,2,14,11,13,12};
  int array_length = 15;

  Heap * max_heap = build_max_heap(create_heap_from_array(array, array_length), array_length);
  printf("\nMax ");
  print_heap(max_heap);

  Heap * min_heap = build_min_heap(create_heap_from_array(array, array_length), array_length);
  printf("\nMin ");
  print_heap(min_heap);

  Heap * ascending_sorted = heap_sort_ascending(array, 15);
  printf("\nAscending Sorted ");
  print_heap(ascending_sorted);

  Heap * descending_sorted = heap_sort_descending(array, 15);
  printf("\nDescending Sorted ");
  print_heap(descending_sorted);

  Heap * priority_queue = create_priority_queue();
  printf("\nPriority Queue\n");
  print_heap(priority_queue);
  priority_queue_insert(priority_queue, 0);
  priority_queue_insert(priority_queue, 1);
  priority_queue_insert(priority_queue, 2);
  priority_queue_insert(priority_queue, 3);
  priority_queue_insert(priority_queue, 4);
  priority_queue_insert(priority_queue, 5);
  print_heap(priority_queue);
  printf("Max: %d\n", priority_queue_get_max(priority_queue));
  print_heap(priority_queue);
  printf("Max Extracted: %d\n", priority_queue_extract_max(priority_queue));
  print_heap(priority_queue);
  printf("Max Extracted: %d\n", priority_queue_extract_max(priority_queue));
  print_heap(priority_queue);
  printf("Max Extracted: %d\n", priority_queue_extract_max(priority_queue));
  print_heap(priority_queue);

  return 0;
}