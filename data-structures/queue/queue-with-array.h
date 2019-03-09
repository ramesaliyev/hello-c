#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct QueueArray {
  int front;
  int rear;
  int size;
  int counter;
  int * data;
} QueueArray;

QueueArray * malloc_queue_array();

QueueArray * create_queue_array(int size);

bool is_queue_array_empty(QueueArray * queue);

bool is_queue_array_full(QueueArray * queue);

void enqueue_array(QueueArray * queue, int data);

int dequeue_array(QueueArray * queue);

void print_queue_array(QueueArray * queue);