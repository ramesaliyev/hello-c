#include "queue-with-array.h"

QueueArray * malloc_queue_array() {
  return (QueueArray *) malloc(sizeof(QueueArray));
}

QueueArray * create_queue_array(int size) {
  int array[size];

  QueueArray * new_queue = malloc_queue_array();
  new_queue->front = -1;
  new_queue->rear = -1;
  new_queue->size = size;
  new_queue->counter = 0;
  new_queue->data = (int *) malloc(sizeof(int) * size);

  return new_queue;
}

bool is_queue_array_empty(QueueArray * queue) {
  return queue->counter == 0;
}

bool is_queue_array_full(QueueArray * queue) {
  return queue->counter == queue->size;
}

void enqueue_array(QueueArray * queue, int data) {
  if (!is_queue_array_full(queue)) {
    queue->rear = (queue->rear + 1) % queue->size;
    queue->data[queue->rear] = data;
    queue->counter++;

    if (queue->front == -1) {
      queue->front = 0;
    }
  }
}

int dequeue_array(QueueArray * queue) {
  if (!is_queue_array_empty(queue)) {
    int data = queue->data[queue->front];
    queue->front = (queue->front + 1) % queue->size;
    queue->counter--;
    return data;
  }

  return 0;
}

void print_queue_array(QueueArray * queue) {
  if (is_queue_array_empty(queue)) {
    printf("Queue is empty.\n");
  } else {
    for (int i = queue->front; i <= queue->rear; i = (i + 1) % queue->size) {
      printf("%d\n", queue->data[i]);
    }
  }
  printf("\n");
}