#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "stack-with-linked-list.h"

StackNode * malloc_stack_node() {
  return (StackNode *) malloc(sizeof(StackNode));
}

StackLinkedList * malloc_stack_list() {
  return (StackLinkedList *) malloc(sizeof(StackLinkedList));
}

StackLinkedList * create_stack_list(int size) {
  StackLinkedList * new_stack = malloc_stack_list();
  new_stack->top = NULL;
  new_stack->size = size;
  new_stack->counter = 0;
  return new_stack;
}

bool is_stack_list_empty(StackLinkedList * stack) {
  return stack->top == NULL;
}

bool is_stack_list_full(StackLinkedList * stack) {
  return stack->counter == stack->size;
}

int peek_stack_list(StackLinkedList * stack) {
  if (!is_stack_list_empty(stack)) {
    return stack->top->data;
  }

  return 0;
}

int pop_stack_list(StackLinkedList * stack) {
  if (!is_stack_list_empty(stack)) {
    StackNode * top = stack->top;
    stack->top = top->next;
    stack->counter--;

    int data = top->data;
    free(top);
    return data;
  }

  return 0;
}

void push_stack_list(StackLinkedList * stack, int data) {
  if (!is_stack_list_full(stack)) {
    StackNode * new_node = malloc_stack_node();
    new_node->next = NULL;
    new_node->data = data;

    if (stack->top != NULL) {
      new_node->next = stack->top;
    }

    stack->top = new_node;
    stack->counter++;
  }
}

void destroy_stack_list(StackLinkedList * stack) {
  if (!is_stack_list_empty(stack)) {
    StackNode * node = stack->top;

    while (node->next != NULL) {
      StackNode * tmp = node;
      node = tmp->next;
      free(tmp);
    }

    free(node);
  }

  free(stack);
}

void print_stack_list (StackLinkedList *stack) {
  StackNode * current_node = stack->top;

  printf("\n");
  while (current_node != NULL) {
    printf("%d\n", current_node->data);
    current_node = current_node->next;
  }
  printf("\n");
}