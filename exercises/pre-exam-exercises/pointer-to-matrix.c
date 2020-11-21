#include "stdio.h"
#include "stdlib.h"

int main() {
  int** mat;
  int n, m, i, j;

  printf("Enter num of rows: ");
  scanf("%d", &n);
  printf("Enter num of columns: ");
  scanf("%d", &m);

  mat = (int **) malloc(n * sizeof(int *));

  if(mat == NULL) {
    printf("%s:%d > Can not allocate memory for the array...\n", __FILE__, __LINE__);
    return -1;
  }

  for(i = 0; i < n; i++) {
    mat[i] = (int *) malloc(m * sizeof(int));
  }

  for(i = 0; i < n; i++) {
    for(j = 0; j < m; j++) {
      printf("Enter value of Mat[%d][%d]: ", i, j);
      scanf("%d", &mat[i][j]);
    }
  }

  for(i = 0; i < n; i++) {
    for(j = 0; j < m; j++) {
      printf("%d ", mat[i][j]);
    }
    printf("\n");
  }

  for(i = 0; i < n; i++) {
    for(j = 0; j < m; j++) {
      printf("%d ", *(*(mat + i) + j));
    }
    printf("\n");
  }

  free(mat);

  return 1;
}
