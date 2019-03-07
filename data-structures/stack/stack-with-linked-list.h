#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct link {
  int data;
  struct link *next;
} StackNode;

typedef struct stackLinkedList {
  StackNode * top;
  int size;
  int counter;
} StackLinkedList;

StackNode * malloc_stack_node();

StackLinkedList * malloc_stack_list();

StackLinkedList * create_stack_list(int size);

bool is_stack_list_empty(StackLinkedList * stack);

bool is_stack_list_full(StackLinkedList * stack);

int peek_stack_list(StackLinkedList * stack);

int pop_stack_list(StackLinkedList * stack);

void push_stack_list(StackLinkedList * stack, int data);

void destroy_stack_list(StackLinkedList * stack);

void print_stack_list (StackLinkedList *stack);
