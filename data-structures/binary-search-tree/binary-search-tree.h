#include <stdio.h>
#include <stdlib.h>

typedef struct node Tree;
struct node {
  int data;
  Tree * left;
  Tree * right;
};

Tree * malloc_tree();

Tree * createTree(int data);

Tree * insert(Tree * T, int data);

Tree * minValueNode(Tree * T);

Tree * delete(Tree * root, int data);

int are_trees_equal(Tree * FirstTree, Tree * SecondTree);

void visit (Tree * T);

void preTraverse(Tree * T);

void inTraverse(Tree * T);

void postTraverse(Tree * T);

void print_with_indent(int indent, char * string);

void printTree(Tree * T, int indent, char * s);
