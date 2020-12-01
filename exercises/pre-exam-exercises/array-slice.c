#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"
#include "math.h"

int* slice(int* arr, int from, int n) {
  int* sliced = (int*) malloc(n * sizeof(int));
  memcpy(sliced, (arr + from), n * sizeof(int));
  return sliced;
}

int main() {
  int arr[] = {1,2,3,4,5,6,7,8,9,10,11,12,13};

  int start = 3;
  int n = 3;
  int* sliced = slice(arr, start, n);

  int i;
  for (i = 0; i < n; i++) {
    printf("%d ", sliced[i]);
  }
  printf("\n");

  return 0;
}
