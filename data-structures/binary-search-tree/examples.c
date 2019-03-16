#include "binary-search-tree.h"

int main(int argc, char **argv) {
  Tree * T1 = createTree(50);
  Tree * T2 = createTree(50);

  printf("Are trees T1 and T2 equal? %d\n", are_trees_equal(T1, T2));

  insert(T1, 10);
  insert(T1, 90);
  insert(T1, 11);
  insert(T1, 80);
  insert(T1, 17);
  insert(T1, 12);
  insert(T1, 31);
  insert(T1, 40);

  printf("\nPrint Tree 1:");
  printTree(T1, 0, "");

  printf("\nPrint Tree 2:");
  printTree(T2, 0, "");

  printf("\npreTraverse T1\n");
  preTraverse(T1);
  printf("\ninTraverse T1\n");
  inTraverse(T1);
  printf("\npostTraverse T1\n");
  postTraverse(T1);

  printf("\nAre trees T1 and T2 equal? %d", are_trees_equal(T1, T2));
  printf("\nInserting same values to T2.\n");

  insert(T2, 10);
  insert(T2, 90);
  insert(T2, 11);
  insert(T2, 80);
  insert(T2, 17);
  insert(T2, 12);
  insert(T2, 31);
  insert(T2, 40);

  printf("\nPrint Tree 1:");
  printTree(T1, 0, "");

  printf("\nPrint Tree 2:");
  printTree(T2, 0, "");

  printf("\nAre trees T1 and T2 equal? %d\n", are_trees_equal(T1, T2));

  printf("\nRemoving 40 from T1:");
  delete(T1, 40);
  printTree(T1, 0, "");

  printf("\nRemoving 17 from T1:");
  delete(T1, 17);
  printTree(T1, 0, "");

  printf("\nRemoving 50 from T1:");
  delete(T1, 50);
  printTree(T1, 0, "");

  return 0;
}
