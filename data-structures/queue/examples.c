#include "queue-with-array.h"
#include "queue-with-linked-list.h"

int queue_array_example() {
  QueueArray * queue = create_queue_array(5);

  enqueue_array(queue, 1);
  enqueue_array(queue, 2);
  enqueue_array(queue, 3);
  enqueue_array(queue, 4);
  enqueue_array(queue, 5);
  enqueue_array(queue, 6); // wont add

  print_queue_array(queue);

  printf("%d\n", dequeue_array(queue));
  printf("%d\n", dequeue_array(queue));
  printf("%d\n", dequeue_array(queue));
  printf("\n");

  enqueue_array(queue, 1);
  enqueue_array(queue, 2);
  enqueue_array(queue, 3);

  print_queue_array(queue);
  printf("\n");

  printf("%d\n", dequeue_array(queue));
  printf("%d\n", dequeue_array(queue));
  printf("%d\n", dequeue_array(queue));
  printf("%d\n", dequeue_array(queue));
  printf("%d\n", dequeue_array(queue));
  printf("\n");

  enqueue_array(queue, 1);
  enqueue_array(queue, 2);
  enqueue_array(queue, 3);
  enqueue_array(queue, 4);
  enqueue_array(queue, 5);
  enqueue_array(queue, 6); // wont add

  print_queue_array(queue);
  printf("\n");

  return 0;
}

int queue_list_example() {
  QueueLinkedList * queue = create_queue_list(5);

  enqueue_list(queue, 1);
  enqueue_list(queue, 2);
  enqueue_list(queue, 3);
  enqueue_list(queue, 4);
  enqueue_list(queue, 5);
  enqueue_list(queue, 6); // wont add

  print_queue_list(queue);

  printf("%d\n", dequeue_list(queue));
  printf("%d\n", dequeue_list(queue));
  printf("%d\n", dequeue_list(queue));
  printf("\n");

  enqueue_list(queue, 1);
  enqueue_list(queue, 2);
  enqueue_list(queue, 3);

  print_queue_list(queue);
  printf("\n");

  printf("%d\n", dequeue_list(queue));
  printf("%d\n", dequeue_list(queue));
  printf("%d\n", dequeue_list(queue));
  printf("%d\n", dequeue_list(queue));
  printf("%d\n", dequeue_list(queue));
  printf("\n");

  enqueue_list(queue, 1);
  enqueue_list(queue, 2);
  enqueue_list(queue, 3);
  enqueue_list(queue, 4);
  enqueue_list(queue, 5);
  enqueue_list(queue, 6); // wont add

  print_queue_list(queue);
  printf("\n");

  destroy_queue_list(queue);

  return 0;
}

int main(int argc, char **argv) {
  printf("\nexamples with queue array\n");
  queue_array_example();
  printf("\nexamples with queue linked list\n");
  queue_list_example();

  return 0;
}