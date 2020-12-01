#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "math.h"

bool isPerfectNumber(int x) {
  int sumOfFactors = 0;
  
  int i;
  for (i = 1; i < x; i++) {
    if (x % i == 0) {
      sumOfFactors += i;
    }
  }

  if (sumOfFactors == x) {
    return true;
  }

  return false;
}

int main() {
  printf("Is 6 perfect? %d \n", isPerfectNumber(6));
  printf("Is 24 perfect? %d \n", isPerfectNumber(24));
  printf("Is 28 perfect? %d \n", isPerfectNumber(28));
  printf("Is 315 perfect? %d \n", isPerfectNumber(315));
  printf("Is 496 perfect? %d \n", isPerfectNumber(496));
  printf("Is 7854 perfect? %d \n", isPerfectNumber(7854));
  printf("Is 8128 perfect? %d \n", isPerfectNumber(8128));

  return 0;
}
