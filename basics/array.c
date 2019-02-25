#include <stdio.h>

int main(int argc, char **argv) {

  // type arrayName [arraySize];
  // type arrayName [ x ][ y ];

  // Define
  int arr1[10];

  // Define and initialize
  int arr2[10] = {0,1,2,3,4,5,6,7,8,9};

  // Define and initialize without size.
  int arr3[] = {0,1,2,3,4,5,6,7,8,9};

  // Get & Set
  arr1[0] = 300;
  printf("%d\n", arr1[0]);

  // Multidimensional

  int marr[3][4] = {
    {0, 1, 2, 3},
    {4, 5, 6, 7},
    {8, 9, 10, 11}
  };

  printf("%d\n", marr[1][1]);

  return 0;
}
