#include <stdio.h>

int main() {
  int varA = 5;
  int varB = 7;

  // Declare a pointer variable.
  // Set value of pointer as address of varA.
  // So it will be point the address of varA.
  int* pointer = &varA;

  // Double pointer.
  // Pointer which points to another pointer.
  int** pointer2 = &pointer;

  // %p is the correct format specifier for pointer.
  printf("Address of varA: %p\n", &varA);
  printf("Address of varB: %p\n", &varB);
  printf("Address of pointer: %p\n", &pointer);
  printf("Address of pointer2: %p\n", &pointer2);
  printf("Address of the pointer which pointer2 points: %p\n", &*pointer2);
  printf("\n");

  // Print value of varA.
  printf("Value of varA: %d\n", varA);
  printf("Value of varB: %d\n", varB);
  printf("\n");

  // Access content of a memory adress by unary * operator.
  printf("Value of pointer: %p\n", pointer);
  printf("Content of memory where pointer is points: %d\n", *pointer);
  printf("\n");

  printf("Change value of varA to 10\n");
  printf("\n");
  varA = 10;

  printf("Value of pointer: %p\n", pointer);
  printf("Content of memory where pointer is points: %d\n", *pointer);
  printf("\n");

  printf("Change content of memory where pointer is points to 11\n");
  printf("\n");
  *pointer = 11;

  printf("Value of pointer: %p\n", pointer);
  printf("Content of memory where pointer is points: %d\n", *pointer);
  printf("\n");

  printf("Change pointer's value to make it point to varB\n");
  printf("\n");
  pointer = &varB;

  printf("Value of pointer: %p\n", pointer);
  printf("Content of memory where pointer is points: %d\n", *pointer);
  printf("\n");

  printf("Value of pointer2: %p\n", pointer2);
  printf("Content of memory where pointer2 is points: %p\n", *pointer2);
  printf("Content of memory where pointer2 is points is points: %d\n", **pointer2);
  printf("\n");

  return 0;
}
