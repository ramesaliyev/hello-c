#include "stdio.h"
#include "stdlib.h"

void set_int(int *p, int val) {
    *p = val;
}

int main() {
  int* int_p = (int *) malloc(sizeof(int));
  set_int(int_p, 17);
  printf("%d\n", *int_p);

  free(int_p);

  return 1;
}
