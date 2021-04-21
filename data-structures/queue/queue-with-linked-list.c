#include "queue-with-linked-list.h"

QueueNode * malloc_queue_node() {
  return (QueueNode *) malloc(sizeof(QueueNode));
}

QueueLinkedList * malloc_queue_list() {
  return (QueueLinkedList *) malloc(sizeof(QueueLinkedList));
}

QueueLinkedList * create_queue_list(int size) {
  QueueLinkedList * new_queue = malloc_queue_list();
  new_queue->front = NULL;
  new_queue->rear = NULL;
  new_queue->size = size;
  new_queue->counter = 0;
  return new_queue;
}

bool is_queue_list_empty(QueueLinkedList * queue) {
  return queue->front == NULL;
}

bool is_queue_list_full(QueueLinkedList * queue) {
  return queue->counter == queue->size;
}

void enqueue_list(QueueLinkedList * queue, int data) {
  if (!is_queue_list_full(queue)) {
    QueueNode * new_node = malloc_queue_node();
    new_node->data = data;
    new_node->next = NULL;

    if (is_queue_list_empty(queue)) {
      queue->front = new_node;
      queue->rear = new_node;
    } else {
      queue->rear->next = new_node;
      queue->rear = new_node;
    }

    queue->counter++;
  }
}

int dequeue_list(QueueLinkedList * queue) {
  if (!is_queue_list_empty(queue)) {
    QueueNode * front = queue->front;
    int val = front->data;
    queue->front = queue->front->next;
    queue->counter--;
    free(front);
    if (queue->front == NULL) {
      queue->rear = NULL;
    }
    return val;
  }

  return 0;
}

void destroy_queue_list(QueueLinkedList * queue) {
  if (!is_queue_list_empty(queue)) {
    QueueNode * node = queue->front;

    while (node->next != NULL) {
      QueueNode * tmp = node;
      node = tmp->next;
      free(tmp);
    }

    free(node);
  }

  free(queue);
}

void print_queue_list(QueueLinkedList * queue) {
  QueueNode * current_node = queue->front;

  printf("\n");
  while (current_node != NULL) {
    printf("%d\n", current_node->data);
    current_node = current_node->next;
  }
  printf("\n");
}
