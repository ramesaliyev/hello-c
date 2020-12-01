#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "math.h"

bool isEven(int x) {
  return x % 2 == 0 ? true : false;
}

bool isPrime(int x) {
  if (x == 1) return false;
  if (x == 2) return true;
  if (isEven(x)) return false;

  int square = floor(sqrt(x));
  if (square <= 2) return true;
    
  int i;
  for (i = 3; i <= square; i+=2) {
    if (x % i == 0) {
      return false;
    }
  }

  return true;
}

int* nextPrime(int x, int n) {
  int* nextNPrimes = (int *) malloc(n * sizeof(int));
  int* cursor = nextNPrimes;
  int foundCount = 0;

  int i = isEven(x) ? x + 1 : x + 2;
  while (foundCount < n) {
    if (isPrime(i) == true) {
      *(cursor) = i;
      cursor++;
      foundCount++;
    }
    i += 2;
  }

  return nextNPrimes;
}

void printNInt(int* x, int n) {
  int i;
  for (i = 0; i < n; i++) {
    printf("%d ", *(x++));
  }
  printf("\n");
}

int main() {
  printf("isPrime(1) %d \n", isPrime(1));
  printf("isPrime(2) %d \n", isPrime(2));
  printf("isPrime(3) %d \n", isPrime(3));
  printf("isPrime(4) %d \n", isPrime(4));
  printf("isPrime(5) %d \n", isPrime(5));
  printf("isPrime(6) %d \n", isPrime(6));
  printf("isPrime(7) %d \n", isPrime(7));
  printf("isPrime(15) %d \n", isPrime(15));
  printf("isPrime(21) %d \n", isPrime(21));
  printf("isPrime(29) %d \n", isPrime(29));
  printf("isPrime(51) %d \n", isPrime(51));
  printf("isPrime(97) %d \n", isPrime(97));

  int* nextNPrimes;
  int n = 5;

  printf("Primes after 2: "); 
  nextNPrimes = nextPrime(2, n);
  printNInt(nextNPrimes, n);

  printf("Primes after 19: "); 
  nextNPrimes = nextPrime(19, n);
  printNInt(nextNPrimes, n);

  printf("Primes after 97: "); 
  nextNPrimes = nextPrime(97, n);
  printNInt(nextNPrimes, n);
  
  free(nextNPrimes);

  return 0;
}
