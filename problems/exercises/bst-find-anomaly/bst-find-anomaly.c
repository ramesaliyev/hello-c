// Find the node that causing anomaly in binary search tree.

#include "stdio.h"
#include "stdlib.h"

typedef struct Tree Tree;

struct Tree {
  Tree * right;
  Tree * left;
  int data;
};

Tree * create_tree(data) {
  Tree * tree = (Tree *) malloc(sizeof(Tree));
  tree->right = NULL;
  tree->left = NULL;
  tree->data = data;
  return tree;
}

Tree * insert_tree(Tree * tree, int data) {
  if (tree == NULL) {
    return create_tree(data);
  }

  if (data > tree->data) {
    tree->right = insert_tree(tree->right, data);
  } else if (data < tree->data) {
    tree->left = insert_tree(tree->left, data);
  }

  return tree;
}

int find_anomaly(Tree * tree) {
  if (tree == NULL) return -1;

  if (tree->left && tree->left->data >= tree->data) {
    return tree->left->data;
  }

  if (tree->right && tree->right->data <= tree->data) {
    return tree->left->data;
  }

  int left_check = find_anomaly(tree->left);
  int right_check = find_anomaly(tree->right);

  if (left_check >= 0) {
    return left_check;
  }

  if (right_check >= 0) {
    return right_check;
  }

  return -1;
}

int main() {
  Tree * tree = create_tree(10);
  insert_tree(tree, 5);
  insert_tree(tree, 17);
  insert_tree(tree, 3);
  insert_tree(tree, 9);
  insert_tree(tree, 13);
  insert_tree(tree, 19);

  printf("%d\n", tree->left->data);
  printf("%d\n", tree->right->data);
  printf("%d\n", tree->left->left->data);
  printf("%d\n", tree->left->right->data);
  printf("%d\n", tree->right->left->data);
  printf("%d\n", tree->right->right->data);

  tree->right->left->data = 19;

  printf("\n\n");
  printf("%d\n", find_anomaly(tree));
}

/*
      10
  5       17
3  9    13  19
*/