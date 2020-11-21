#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"

typedef struct Node Node;
struct Node {
  int val;
  Node* next;
};

Node* create_node(int val) {
  Node* node = (Node *) malloc(sizeof(Node));
  node->val = val;
  node->next = NULL;
  return node;
}

void insert_next(Node* head, int val) {
  Node* next = create_node(val);
  Node* last = head;
  
  while (last->next) {
    last = last->next;
  }

  last->next = next;
}

void insert_after(Node* head, int search, int val) {
  Node* after = create_node(val);
  Node* node = head;
  
  bool found = false;
  while (!found) {
    if (node->val == search || node->next == NULL) {
      found = true;
      after->next = node->next;
      node->next = after;
    } else {
      node = node->next;
    }
  }
}

Node* reverse_list(Node* head) {
  Node* current = head;
  Node* next = current->next;
  Node* temp;

  while(next) {
    temp = next->next;
    next->next = current;
    current = next;
    next = temp;
  }

  head->next = NULL;
  return current;
}

void free_list(Node* head) {
  Node* node = head;
  Node* temp;
  
  while (node) {
    temp = node->next;
    free(node);
    node = temp;
  }
}

void print_list(Node* head) {
  Node* node = head;
  while (node) {
    printf("%d ", node->val);
    node = node->next;
  }
  printf("\n");
}

int main() {
  Node* head = create_node(0);
  insert_next(head, 1);
  insert_next(head, 4);
  insert_next(head, 5);
  insert_next(head, 6);
  insert_after(head, 1, 2);
  insert_after(head, 2, 3);
  print_list(head);
  head = reverse_list(head);
  print_list(head);
  
  free_list(head);

  return 1;
}
