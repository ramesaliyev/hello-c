#include <stdio.h>


int main(int argc, char **argv) {
   // All sizes are compiler/system/architecture dependent.

   // 1
   printf("sizeof char: %lu byte\n", sizeof(char));
   printf("sizeof unsigned char: %lu byte\n", sizeof(unsigned char));
   printf("sizeof signed char: %lu byte\n", sizeof(signed char));
   printf("\n");

   // 2
   printf("sizeof short: %lu byte\n", sizeof(short));
   printf("sizeof unsigned char: %lu byte\n", sizeof(unsigned char));
   printf("sizeof signed char: %lu byte\n", sizeof(signed char));
   printf("\n");

   // 4
   printf("sizeof int: %lu byte\n", sizeof(int));
   printf("sizeof unsigned int: %lu byte\n", sizeof(unsigned int));
   printf("sizeof signed int: %lu byte\n", sizeof(signed int));
   printf("\n");

   // 8
   printf("sizeof long int: %lu byte\n", sizeof(long int));
   printf("sizeof long unsigned int: %lu byte\n", sizeof(long unsigned int));
   printf("sizeof long signed int: %lu byte\n", sizeof(long signed int));
   printf("\n");

   // 4
   printf("sizeof float: %lu byte\n", sizeof(float));
   printf("\n");

   // 8
   printf("sizeof long: %lu byte\n", sizeof(long));
   printf("sizeof unsigned long: %lu byte\n", sizeof(unsigned long));
   printf("sizeof signed long: %lu byte\n", sizeof(signed long));
   printf("\n");

   // 8
   printf("sizeof long long: %lu byte\n", sizeof(long long));
   printf("sizeof unsigned long long: %lu byte\n", sizeof(unsigned long long));
   printf("sizeof signed long long: %lu byte\n", sizeof(signed long long));
   printf("\n");

   // 8
   printf("sizeof double: %lu byte\n", sizeof(double));
   printf("\n");

   // 16
   printf("sizeof long double: %lu byte\n", sizeof(long double));
   printf("\n");

   return 0;
}
