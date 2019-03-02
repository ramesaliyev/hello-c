#include <stdio.h>
#include <stdlib.h>
#include "linked-list.h"

/**
 * Main program.
 */
int main(int argc, char **argv) {
  int array[10] = {0,1,2,3,4,5,6,7,8,9};
  Node * list = createLinkedListfromArray(array, sizeof(array));

  printf("List created from an array.");
  printLinkedList(list);

  printf("Reversed.");
  list = reverse(list);
  printLinkedList(list);

  printf("Insert 10 to top.");
  list = insert_top(10, list);
  printLinkedList(list);

  printf("Insert value 55 after value 5.");
  list = insert_after_value(55, 5, list);
  printLinkedList(list);

  printf("Insert value 55 after value 5.");
  list = delete_nodes_wvalue(6, list);
  printLinkedList(list);

  return 0;
}