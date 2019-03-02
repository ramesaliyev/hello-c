#ifndef LINKED_LIST /* Include guard */
#define LINKED_LIST

typedef struct link {
  int data;
  struct link *next;
} Node;

Node * malloc_node();
Node * insert_top(int num, Node *head);
Node * insert_after_value(int num, int prevnum, Node * head);
Node * delete_nodes_wvalue(int num, Node * head);
Node * reverse(Node *current_head);
Node * createLinkedListfromArray(int *array, long size);
void printLinkedList (Node *head);

#endif