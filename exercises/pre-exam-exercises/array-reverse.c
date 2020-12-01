#include "stdio.h"
#include "stdlib.h"

int main() {
  int arr[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  int n = sizeof(arr) / sizeof(arr[0]);

  int i;
  int temp;
  for (i = 0; i < n/2; i++) {
    temp = arr[n - 1 - i];
    arr[n -1 -i] = arr[i];
    arr[i] = temp;     
  }

  int j;
  for (j = 0; j < n; j++) {
    printf("%d ", arr[j]);
  }

  printf("\n");

  return 0;
}
