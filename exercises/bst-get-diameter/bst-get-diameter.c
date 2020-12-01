#include "stdio.h"
#include "stdlib.h"

typedef struct Tree Tree;
struct Tree {
  Tree * left;
  Tree * right;
  int data;
};

Tree * create_tree(int data) {
  Tree * tree = (Tree *) malloc(sizeof(Tree));
  tree->left = NULL;
  tree->right = NULL;
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

int max(int a, int b) {
  return a > b ? a : b;
}

int height(Tree * tree) {
  if (tree == NULL) return 0;
  return 1 + max(height(tree->left), height(tree->right));
}

int diameter(Tree * tree) {
  if (tree == NULL) return 0;

  int height_left = height(tree->left);
  int height_right = height(tree->right);
  int diameter_left = diameter(tree->left);
  int diameter_right = diameter(tree->right);

  return max(height_left + height_right + 1, max(diameter_left, diameter_right));
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

  insert(tree, 23);
  insert(tree, 27);
  insert(tree, 21);

  printf("Height:%d\n", height(tree));
  printf("Diameter:%d\n", diameter(tree));

  return 0;
}

/*
                  11
          5                  17
      3       9          13     19
    1  4    8  10      12  15  18  25
*/