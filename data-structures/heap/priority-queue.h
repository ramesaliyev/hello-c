#include "max-heap.h"
#include "min-heap.h"

Heap * create_priority_queue();

int priority_queue_get_max(Heap * heap);

int priority_queue_extract_max(Heap * heap);

void priority_queue_increase(Heap * heap, int index, int value);

void priority_queue_insert(Heap * heap, int value);