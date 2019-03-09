#include "linked-list.h"

/**
 * Allocate memory for a node.
 */
Node * malloc_node() {
  return (Node *) malloc(sizeof(Node));
}

/**
 * Insert at the top of the linked list.
 */
Node * insert_top(int num, Node *head) {
  Node *new_node;
  new_node = malloc_node();
  new_node->data = num;
  new_node->next = head;
  head = new_node;
  return head;
}

/**
 * Insert after node which has given value.
 */
Node * insert_after_value(int num, int prevnum, Node * head) {
  Node * current_node = head;
  Node * new_node;

  while(current_node->data != prevnum) {
    current_node = current_node->next;
  }

  new_node = malloc_node();
  new_node->data = num;
  new_node->next = current_node->next;
  current_node->next = new_node;

  return head;
}

/**
 * Delete nodes which has given value.
 */
Node * delete_nodes_wvalue(int num, Node * head) {
  Node * prev_node;
  Node * current_node = head;

  while(current_node != NULL) {
    if (current_node->data == num) {
      if (current_node == head) {
        head = current_node-> next;
      } else {
        prev_node->next = current_node->next;
      }

      free(current_node);
    }

    prev_node = current_node;
    current_node = current_node->next;
  }

  return head;
}

/**
 * Reverse given linked list.
 */
Node * reverse(Node *current_head) {
    Node *new_head = NULL;
    while (current_head != NULL) {
      Node *next = current_head->next;
      current_head->next = new_head;
      new_head = current_head;
      current_head = next;
    }
    return new_head;
}

/**
 * Create a linked list from an array.
 */
Node * createLinkedListfromArray(int *array, long size) {
  Node * head = malloc_node();
  Node * current_node = head;

  int length = size / sizeof(array[0]);

  for (int i = 0; i < length; i++) {
    current_node->data = array[i];

    if (i == length - 1) {
      current_node->next = NULL;
    } else {
      Node * new_node = malloc_node();
      current_node->next = new_node;
      current_node = new_node;
    }
  }

  return head;
}

/**
 *  Print a linked list.
 */
void printLinkedList (Node *head) {
  Node * current_node = head;

  printf("\n");
  while (current_node != NULL) {
    printf("%d\n", current_node->data);
    current_node = current_node->next;
  }
  printf("\n");
}
