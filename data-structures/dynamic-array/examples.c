#include "dynamic-array.h"

int main(int argc, char **argv) {
  Array * array = create_array(2);

  insert_array(array, 1);
  insert_array(array, 2);
  insert_array(array, 3);
  insert_array(array, 4);
  insert_array(array, 5);
  insert_array(array, 6);
  insert_array(array, 7);
  insert_array(array, 8);
  insert_array(array, 9);
  insert_array(array, 10);

  print_array(array);

  int arr[10] = {0,1,2,3,4,5,6,7,8,9};
  Array * array2 = create_array_from_array(arr, 10);
  print_array(array2);

  swap_array(array2, 1, 3);
  print_array(array2);

  printf("Item on index %dth element of array: %d\n", 5, get_array(array2, 5));

  return 0;
}
