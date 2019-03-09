#include "stack-with-array.h"

StackArray * malloc_stack_array() {
  return (StackArray *) malloc(sizeof(StackArray));
}

StackArray * create_stack_array(int size) {
  int array[size];

  StackArray * new_stack = malloc_stack_array();
  new_stack->top = -1;
  new_stack->size = size;
  new_stack->data = (int *) malloc(sizeof(int) * size);

  return new_stack;
}

bool is_stack_array_empty(StackArray * stack) {
  return stack->top == -1;
}

bool is_stack_array_full(StackArray * stack) {
  return stack->top == stack->size - 1;
}

int peek_stack_array(StackArray * stack) {
  if (!is_stack_array_empty(stack)) {
    return stack->data[stack->top];
  }

  return 0;
}

int pop_stack_array(StackArray * stack) {
  int data;

  if (!is_stack_array_empty(stack)) {
    data = stack->data[stack->top];
    stack->top--;
    return data;
  }

  return 0;
}

void push_stack_array(StackArray * stack, int data) {
  if (!is_stack_array_full(stack)) {
    stack->top++;
    stack->data[stack->top] = data;
  }
}

void print_stack_array(StackArray * stack) {
  if (is_stack_array_empty(stack)) {
    printf("Stack is empty.\n");
  } else {
    for (int i = stack->top; i >= 0; i--) {
      printf("%d\n", stack->data[i]);
    }

    printf("\n");
  }
}