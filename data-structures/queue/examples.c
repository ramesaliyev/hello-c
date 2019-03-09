#include "queue-with-array.h"

int main(int argc, char **argv) {
  QueueArray * queue = create_queue_array(10);

  enqueue_array(queue, 1);
  enqueue_array(queue, 2);
  enqueue_array(queue, 3);
  enqueue_array(queue, 4);
  enqueue_array(queue, 5);

  print_queue_array(queue);

  printf("%d\n", dequeue_array(queue));
  printf("%d\n", dequeue_array(queue));
  printf("%d\n", dequeue_array(queue));
  printf("\n");

  print_queue_array(queue);

  return 0;
}