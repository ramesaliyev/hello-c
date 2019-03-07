#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct StackArray {
  int top;
  int size;
  int * data;
} StackArray;

StackArray * malloc_stack_array();

StackArray * create_stack_array(int size);

bool is_stack_array_empty(StackArray * stack);

bool is_stack_array_full(StackArray * stack);

int peek_stack_array(StackArray * stack);

int pop_stack_array(StackArray * stack);

void push_stack_array(StackArray * stack, int data);

void print_stack_array(StackArray * stack);
