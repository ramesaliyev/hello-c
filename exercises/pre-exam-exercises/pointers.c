#include "stdio.h"

int main() {
  int q;
  int t;

  q = 7;
  t = 9;

  int* p;

  p = &q;
  q = 3;

  // will print 3
  printf("*p = %d \n", *p);

  int** pp;
  pp = &p;
  q = 5;
  
  // will print 5
  printf("**pp = %d \n", **pp);

  p = &t;

  // will print 9
  printf("**pp = %d \n", **pp);

  return 0;
}
