#include "stdio.h"
#include "stdlib.h"

int main() {
  int* p_arr;

  int n;
  printf("Enter size of arr: ");
  scanf("%d", &n);
  p_arr = (int *) calloc(n, sizeof(int));

  int i;
  for (i = 0; i<n; i++) {
    printf("Enter [%d]th element: ", i);
    scanf("%d", (p_arr + i));
  }

  for (i = 0; i<n; i++) {
    printf("p_arr[%d] = %d \n", i, *(p_arr + i));
  }

  free(p_arr);

  return 0;
}
