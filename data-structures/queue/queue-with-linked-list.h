#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct link {
  int data;
  struct link *next;
} QueueNode;

typedef struct queueLinkedList {
  QueueNode * front;
  QueueNode * rear;
  int size;
  int counter;
} QueueLinkedList;

QueueNode * malloc_queue_node();

QueueLinkedList * malloc_queue_list();

QueueLinkedList * create_queue_list(int size);

bool is_queue_list_empty(QueueLinkedList * queue);

bool is_queue_list_full(QueueLinkedList * queue);

void enqueue_list(QueueLinkedList * queue, int data);

int dequeue_list(QueueLinkedList * queue);

void destroy_queue_list(QueueLinkedList * queue);

void print_queue_list(QueueLinkedList * queue);