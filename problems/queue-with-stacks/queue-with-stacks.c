#include "stdio.h"
#include "stdlib.h"

typedef struct Node Node;
typedef struct Stack Stack;
typedef struct Queue Queue;

typedef struct Node {
  int data;
  Node * next;
} Node;

typedef struct Stack {
  Node * top;
  int length;
} Stack;

typedef struct Queue {
  Stack * stack_main;
  Stack * stack_temp;
  int length;
} Queue;

Stack * create_stack() {
  Stack * stack = (Stack *) malloc(sizeof(Stack));
  stack->length = 0;
  return stack;
}

Queue * create_queue() {
  Queue * queue = (Queue *) malloc(sizeof(Queue));
  queue->length = 0;
  queue->stack_main = create_stack();
  queue->stack_temp = create_stack();
  return queue;
}

void stack_push(Stack * stack, int data) {
  Node * new_node = (Node *) malloc(sizeof(Node));
  new_node->data = data;
  new_node->next = stack->top;
  stack->top = new_node;
  stack->length++;
}

int stack_pop(Stack * stack) {
  Node * top = stack->top;
  int data = top->data;
  stack->top = top->next;
  stack->length--;
  free(top);
  return data;
}

void queue_push(Queue * queue, int data) {
  Stack * main = queue->stack_main;
  Stack * temp = queue->stack_temp;

  int length = queue->length;
  int i = length;
  while (i--) stack_push(temp, stack_pop(main));
  stack_push(main, data);
  i = length;
  while (length--) stack_push(main, stack_pop(temp));
  queue->length++;
}

int queue_pop(Queue * queue) {
  int data = stack_pop(queue->stack_main);
  queue->length--;
  return data;
}

int main() {
  Queue * queue = create_queue();

  queue_push(queue, 0);
  printf("%d\n", queue_pop(queue));
  queue_push(queue, 1);
  printf("%d\n", queue_pop(queue));
  queue_push(queue, 2);
  queue_push(queue, 3);
  queue_push(queue, 4);
  queue_push(queue, 5);
  printf("%d\n", queue_pop(queue));
  printf("%d\n", queue_pop(queue));
  printf("%d\n", queue_pop(queue));
  printf("%d\n", queue_pop(queue));

  return 0;
}