#include "stdio.h"

// arr[0] is actually syntactic shorthand for *(arr+0)

int main() {
  int arr[] = {1,2,3,4,5};
  int* p;

  p = arr; // arr will decay to pointer

  printf("%d\n", *p);
  printf("%d\n", *(p+1));
  printf("%d\n", *(++p + 1));
  printf("%d\n", *(++p));

  return 0;
}
