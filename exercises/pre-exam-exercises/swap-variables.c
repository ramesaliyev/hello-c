#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "math.h"

void swap(int* p, int* q) {
  int temp = *p;
  *p = *q;
  *q = temp;
}

int main() {
  int a = 10;
  int b = 20;

  swap(&a, &b);

  printf("a=%d, b=%d \n", a, b);

  return 0;
}
