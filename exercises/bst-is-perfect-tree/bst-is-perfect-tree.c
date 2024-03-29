#include "stdio.h"
#include "stdlib.h"

typedef struct Tree Tree;
struct Tree {
  Tree * left;
  Tree * right;
  int data;
};

Tree * create_tree(data) {
  Tree * tree = (Tree *) malloc(sizeof(Tree));
  tree->right = NULL;
  tree->left = NULL;
  tree->data = data;
  return tree;
}

Tree * insert(Tree * tree, int data) {
  if (tree == NULL) return create_tree(data);

  if (data > tree->data) {
    tree->right = insert(tree->right, data);
  } else if (data < tree->data) {
    tree->left = insert(tree->left, data);
  }

  return tree;
}

int isPerfectTree(Tree * tree, int depth) {
  int level = 0;

  if (tree == NULL) return depth;

  int isLeftPerfectTree = isPerfectTree(tree->left, depth + 1);
  int isRightPerfectTree = isPerfectTree(tree->right, depth + 1);

  if (isLeftPerfectTree == isRightPerfectTree && isLeftPerfectTree != -1) {
    return isLeftPerfectTree;
  }

  return -1;
}

int main() {
  Tree * tree = create_tree(11);
  insert(tree, 5);
  insert(tree, 17);
  insert(tree, 3);
  insert(tree, 9);
  insert(tree, 13);
  insert(tree, 19);
  insert(tree, 1);
  insert(tree, 4);
  insert(tree, 8);
  insert(tree, 10);
  insert(tree, 12);
  insert(tree, 15);
  insert(tree, 18);
  insert(tree, 25);

  int isAligned = isPerfectTree(tree, 0);
  isAligned == -1 ? printf("Not aligned.\n") : printf("Aligned.\n");

  return 0;
}

/*
                  11
          5                  17
      3       9          13     19
    1  4    8  10      12  15  18  25
*/