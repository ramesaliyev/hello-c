#include "binary-search-tree.h"

Tree * malloc_tree() {
  return (Tree *) malloc(sizeof(Tree));
};

Tree * createTree(int data) {
  Tree * T = malloc_tree();
  T->data = data;
  T->left = NULL;
  T->right = NULL;
  return T;
};

Tree * insert(Tree * T, int data) {
  if (T == NULL) return createTree(data);

  if (data < T->data) {
    T->left = insert(T->left, data);
  } else if (data > T->data) {
    T->right = insert(T->right, data);
  }

  return T;
}

Tree * minValueNode(Tree * T) {
  Tree * current = T;

  while (current->left != NULL) {
    current = current->left;
  }

  return current;
}

Tree * delete(Tree * root, int data) {
  if (root == NULL) return root;

  if (data < root->data) {
    root->left = delete(root->left, data);
  } else if (data > root->data) {
    root->right = delete(root->right, data);
  } else {
    if (root->left == NULL) {
      Tree * tmp = root->right;
      free(root);
      return tmp;
    } else if (root->right == NULL) {
      Tree * tmp = root->left;
      free(root);
      return tmp;
    } else {
      Tree * tmp = minValueNode(root->right);
      root->data = tmp->data;
      root->right = delete(root->right, tmp->data);
    }
  }

  return root;
}

int are_trees_equal(Tree * FirstTree, Tree * SecondTree) {
  return (
    (FirstTree == NULL && SecondTree == NULL) || // Handle both being NULL.
    (FirstTree && SecondTree &&
      FirstTree->data == SecondTree->data &&
      are_trees_equal(FirstTree->left, SecondTree->left) &&
      are_trees_equal(FirstTree->right, SecondTree->right)
    )
  );
};

void visit (Tree * T) {
  printf("%d\n", T->data);
};

void preTraverse(Tree * T) {
  if (T == NULL) return;
  visit(T);
  preTraverse(T->left);
  preTraverse(T->right);
};

void inTraverse(Tree * T) {
  if (T == NULL) return;
  inTraverse(T->left);
  visit(T);
  inTraverse(T->right);
};

void postTraverse(Tree * T) {
  if (T == NULL) return;
  postTraverse(T->left);
  postTraverse(T->right);
  visit(T);
};

void print_with_indent(int indent, char * string) {
  printf("%*s%s", indent, "", string);
}

void printTree(Tree * T, int indent, char * s) {
  if (T == NULL) return;

  if (indent == 0) {
    printf("\nTree START");
  } else {
    printf("\n | ");
  }

  print_with_indent(indent, "");
  printf("%s\n | ", s);
  print_with_indent(indent, "");
  printf("(%d)", T->data);

  printTree(T->left, indent + 4, "/");
  printTree(T->right, indent + 4, "\\");

  if (indent == 0) printf("\nTree END\n");
};
