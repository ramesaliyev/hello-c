#include "linked-list.h"

int main(int argc, char **argv) {
  int N = 9;
  int M = 5;

  Node * head = malloc_node();
  Node * current_node = head;

  head->data = 1;
  head->next = head;

  for (int i = 2; i <= N; i++) {
    current_node = (current_node->next = malloc_node());
    current_node->data = i;
    current_node->next = head;
  }

  current_node = head;

  while (current_node != current_node->next) {
    for (int j = 1; j <= M; j++) {
      current_node = current_node->next;
    }
    current_node->next = current_node->next->next;
  }

  printf("N=%d, M=%d \n", N, M);
  printf("Result: %d\n", current_node->data);

  return 0;
}