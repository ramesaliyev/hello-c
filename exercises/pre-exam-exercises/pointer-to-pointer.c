#include "stdio.h"

int main() {
  int r = 0;
  int* p = &r;
  int* t = p;
  int** q = &p;
  int*** z = &q;

  r = 1;
  printf("%d %d %d %d %d\n", r, *p, *t, **q, ***z);
  *p = 2;
  printf("%d %d %d %d %d\n", r, *p, *t, **q, ***z);
  *t = 3;
  printf("%d %d %d %d %d\n", r, *p, *t, **q, ***z);
  **q = 4;
  printf("%d %d %d %d %d\n", r, *p, *t, **q, ***z);
  ***z = 5;
  printf("%d %d %d %d %d\n", r, *p, *t, **q, ***z);

  return 0;
}
