// Pre-processor commands.
// These lines are processed by the pre-processor.
// This tells the compiler to literally include the stdio.h file
// into your application. The compiler will find this file in
// the standard locations to look for include files and literally
// “paste” it there where the include line is. This file provides
// some “standard I/O” features, such as printf().
#include <stdio.h>

// This function must exist only once in the application
// because this is the function that is run AS the application.
// When this function exits, the application does.
int main(int argc, char **argv) {
   printf("Hello world!\n");

   // The main() function always returns an integer value.
   // For success return 0.
   // Any other value is an indicator of failure.
   return 0;
}
