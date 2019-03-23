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

  return 0;
}
