#include "stack-with-array.h"
#include "stack-with-linked-list.h"

void stack_array_example() {
  StackArray * stack = create_stack_array(5);

  if (is_stack_array_empty(stack)) {
    printf("Initial stack is empty.\n");
  }

  push_stack_array(stack, 0);
  push_stack_array(stack, 1);
  push_stack_array(stack, 2);
  push_stack_array(stack, 3);
  push_stack_array(stack, 4);
  push_stack_array(stack, 5); // wont push

  print_stack_array(stack);

  printf("%d\n", peek_stack_array(stack));
  printf("%d\n", peek_stack_array(stack));
  printf("\n");

  printf("%d\n", pop_stack_array(stack));
  printf("%d\n", peek_stack_array(stack));
  printf("\n");

  push_stack_array(stack, 5);

  printf("%d\n", peek_stack_array(stack));
  printf("%d\n", pop_stack_array(stack));
}

void stack_list_example() {
  StackLinkedList * stack = create_stack_list(5);

  if (is_stack_list_empty(stack)) {
    printf("Initial stack is empty.\n");
  }

  push_stack_list(stack, 0);
  push_stack_list(stack, 1);
  push_stack_list(stack, 2);
  push_stack_list(stack, 3);
  push_stack_list(stack, 4);
  push_stack_list(stack, 5); // wont push

  print_stack_list(stack);

  printf("%d\n", peek_stack_list(stack));
  printf("%d\n", peek_stack_list(stack));
  printf("\n");

  printf("%d\n", pop_stack_list(stack));
  printf("%d\n", peek_stack_list(stack));
  printf("\n");

  push_stack_list(stack, 5);

  printf("%d\n", peek_stack_list(stack));
  printf("%d\n", pop_stack_list(stack));

  destroy_stack_list(stack);
}

int main(int argc, char **argv) {
  printf("\nexamples with stack array\n");
  stack_array_example();
  printf("\nexamples with stack linked list\n");
  stack_list_example();

  return 0;
}