#include "stdio.h"
#include "stdlib.h"

typedef struct Array {
  int size;
  int* data;
} Array;

Array * concat_arrays(int* a, int a_len, int* b, int b_len) {
  Array* array = (Array *) malloc(sizeof(Array));
  array->size = a_len + b_len;
  array->data = (int *) malloc(array->size * sizeof(int));

  int i;
  
  for (i = 0; i < a_len; i++) {
    *(array->data + i) = a[i];
  }

  for (i = 0; i < b_len; i++) {
    *(array->data + i + a_len) = b[i];
  }

  return array;
}

int main() {
  int a[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  int b[] = {10, 11, 12, 13, 14, 15, 16, 17, 18, 19};

  Array* merged = concat_arrays(a, 10, b, 10);

  int i;
  for (i = 0; i < merged->size; i++) {
    printf("%d ", merged->data[i]);
  }

  printf("\n");
  free(merged);

  return 1;
}
