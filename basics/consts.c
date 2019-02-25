#include <stdio.h>

// The #define Preprocessor
#define A 10
#define B 'B'

int main(int argc, char **argv) {
  // The const Keyword
  const int C = 5;

  printf("A:%d\n", A);
  printf("B:%c\n", B);
  printf("C:%d\n", C);

  return 0;
}
