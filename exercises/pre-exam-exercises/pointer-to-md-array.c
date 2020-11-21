#include "stdio.h"

#define ROW 3
#define COLUMN 5

void set(int* p, int i, int j, int val, int method) {
  int current = *(p + (i*COLUMN) + j);
  *(p + (i*COLUMN) + j) = current + val;
}

int main() {
  int arr[ROW][COLUMN];

  int i, j;
  for (i=0; i < ROW; i++) {
    for (j=0; j < COLUMN; j++) {
      int val = i * COLUMN + j;

      // all of below is same.

      arr[i][j] = val;

      *(*(arr + i) + j) = arr[i][j] + val;

      // important!! not arr, arr[0]. will decay
      set(arr[0], i, j, val, 1);
    } 
  }

  for (i=0; i < ROW; i++) {
    for (j=0; j < COLUMN; j++) {
      printf("%d ", arr[i][j]);  
    } 
    printf("\n");
  }

  return 1;
}
