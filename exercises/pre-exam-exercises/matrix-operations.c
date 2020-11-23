#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "math.h"

typedef struct Matrix {
  int rowc;
  int columnc;
  int** data;
} Matrix;

void print_matrix(Matrix* mat);

// arr[0] is actually syntactic shorthand for *(arr+0)
// *(*(C->data + i) + j) IS EQUAL TO C->data[i][j]
// We are using former version to get better understanding.

Matrix* create_matrix(int n, int m) {
  Matrix* mat = (Matrix*) malloc(sizeof(Matrix));
  mat->rowc = n;
  mat->columnc = m;

  mat->data = (int **) calloc(mat->rowc, sizeof(int *));

  if (mat->data == NULL) {
    return NULL;
  }

  int i;
  for (i = 0; i < mat->rowc; i++) {
    *(mat->data + i) = (int *) calloc(mat->columnc, sizeof(int));
  }

  return mat;
}

Matrix* matrix_from_array(int* arr, int n, int rowc) {
  int columnc = n / rowc;
  Matrix* mat = create_matrix(rowc, columnc);

  int i;
  for (i = 0; i < n; i++) {
    int r = floor(i / columnc);
    int c = i % columnc;
    *(*(mat->data + r) + c) = arr[i];
  }

  return mat;
}

Matrix* sum_matrices(Matrix* A, Matrix* B) {
  if (A->rowc != B->rowc || A->columnc != B->columnc) {
    return NULL;
  }

  Matrix* C = create_matrix(A->rowc, A->columnc);

  int i, j;
  for(i = 0; i < C->rowc; i++) {
    for(j = 0; j < C->columnc; j++) {
      *(*(C->data + i) + j) = *(*(A->data + i) + j) + *(*(B->data + i) + j);
    }
  }

  return C;
}

Matrix* mul_matrices(Matrix* A, Matrix* B) {
  if (A->columnc != B->rowc) {
    return NULL;
  }

  Matrix* C = create_matrix(A->rowc, B->columnc);
  
  int n = A->columnc;
  int i, j, k;
  
  for (i = 0; i < A->rowc; i++) {
    for (j = 0; j < B->columnc; j++) {
      for (k = 0; k < n; k++) {
        C->data[i][j] += A->data[i][k] * B->data[k][j];
        // or
        // *(*(C->data + i) + j) += *(*(A->data + i) + k) * *(*(B->data + k) + j);
      }
    }  
  }

  return C;
}

bool are_matrices_equal(Matrix* A, Matrix* B) {
  if (A->rowc != B->rowc || A->columnc != B->columnc) {
    return false;
  }

  int i, j;

  for (i = 0; i < A->rowc; i++) {
    for (j = 0; j < A->columnc; j++) {
      if (A->data[i][j] != B->data[i][j]) {
        return false;
      }      
    }  
  }

  return true;
}

void fill_matrix(Matrix* mat, int x) {
  int i, j;
  for(i = 0; i < mat->rowc; i++) {
    for(j = 0; j < mat->columnc; j++) {
      *(*(mat->data + i) + j) = x;
    }
  }
}

void print_matrix(Matrix* mat) {
  int i, j;
  printf("Matrix rows:%d columns:%d \n", mat->rowc, mat->columnc);
  for(i = 0; i < mat->rowc; i++) {
    for(j = 0; j < mat->columnc; j++) {
      printf("%d ", *(*(mat->data + i) + j));
    }
    printf("\n");
  }
}

int main() {
  printf("Example 1, create & fill matrix:\n");
  Matrix* mat1 = create_matrix(3, 3);
  print_matrix(mat1);
  fill_matrix(mat1, 10);
  print_matrix(mat1);

  printf("\nExample 2, matrix from array:\n");
  int arr[] = {1,2,3,4,5,6,7,8,9};
  Matrix* mat2 = matrix_from_array(arr, 9, 3);
  print_matrix(mat2);

  printf("\nExample 3, sum of matrices:\n");
  Matrix* mat_sum12 = sum_matrices(mat1, mat2);
  printf("A:\n");
  print_matrix(mat1);
  printf("B:\n");
  print_matrix(mat2);
  printf("A+B:\n");
  print_matrix(mat_sum12);
  
  printf("\nExample 4, multiplication of matrices:\n");
  int arr2[] = {1,2,3,4,5,6};
  int arr3[] = {22,28,49,64};
  Matrix* mat3 = matrix_from_array(arr2, 6, 2);
  Matrix* mat4 = matrix_from_array(arr2, 6, 3);
  printf("A:\n");
  print_matrix(mat3);
  printf("B:\n");
  print_matrix(mat4);
  Matrix* mat_mul34 = mul_matrices(mat3, mat4);
  printf("AxB:\n");
  print_matrix(mat_mul34);

  printf("\nExample 5, are matrices equal:\n");
  printf("A:\n");
  Matrix* mat5 = matrix_from_array(arr3, 4, 2); 
  print_matrix(mat5);
  printf("B:\n");
  print_matrix(mat_mul34);
  printf("Is A == B ?: %d\n", are_matrices_equal(mat5, mat_mul34));
  
  int arr4[] = {45,23,12,45};
  int arr5[] = {22,28,49,64};
  printf("Should say yes: %d\n", are_matrices_equal(matrix_from_array(arr4, 4, 2), matrix_from_array(arr4, 4, 2)));
  printf("Should say no: %d\n", are_matrices_equal(matrix_from_array(arr4, 4, 2), matrix_from_array(arr5, 4, 2)));

  free(mat1);
  free(mat2);
  free(mat3);
  free(mat4);
  free(mat5);
  free(mat_sum12);
  free(mat_mul34);

  return 1;
}
