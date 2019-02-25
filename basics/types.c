#include <stdio.h>
#include <limits.h>

int main(int argc, char **argv) {

   // All sizes are compiler/system/architecture dependent.

   // 1. Basic types.
      // 1.1 Integer Types
      char _char = 127; // 1 byte | -128 to 127 or 0 to 255
      unsigned char _unsigned_char = 255; // 1 byte | 0 to 255
      signed char _signed_char = -128; // 1 byte | -128 to 127

      short _short = 32767; // 2 byte | -32,768 to 32,767 or 0 to 65,535
      unsigned short _unsigned_short = 65535; // 2 byte | 0 to 65,535
      signed short _signed_short = -32768; // 2 byte | -32,768 to 32,767

      int _int = 2147483647; // 4 byte | -2,147,483,648 to 2,147,483,647 or 0 to 4,294,967,295
      unsigned int _unsigned_int = 4294967295; // 4 byte | 0 to 4294,967,295
      signed int _signed_int = -2147483648; // 4 byte | -2,147,483,648 to 2,147,483,647

      long int _long_int = 2147483647; // 8 byte | -2,147,483,648 to 2,147,483,647 or 0 to 4,294,967,295
      unsigned long int _unsigned_long_int = 4294967295; // 8 byte | 0 to 4294,967,295
      signed long int _signed_long_int = -2147483648; // 8 byte | -2,147,483,648 to 2,147,483,647

      long int _long = 2147483647; // 8 byte | -2,147,483,648 to 2,147,483,647 or 0 to 4,294,967,295
      unsigned long int _unsigned_long = 4294967295; // 8 byte | 0 to 4294,967,295
      signed long int _signed_long = -2147483648; // 8 byte | -2,147,483,648 to 2,147,483,647

      long int _long_long = 2147483647; // 8 byte | -2,147,483,648 to 2,147,483,647 or 0 to 4,294,967,295
      unsigned long int _unsigned_long_long = 4294967295; // 8 byte | 0 to 4294,967,295
      signed long int _signed_long_long = -2147483648; // 8 byte | -2,147,483,648 to 2,147,483,647

      // 1.2 Floating Point Types
      float _float = -3.4E38; // 4 byte | -3.4x10^38 to +3.4x10^38

      double _double = -1.7E308; // 8 byte | -1.7x10^308 to +1.7x10^308

      long double _long_double = 1; // 16 byte | ???

   // 2. Enumerated Types

   // 3. The Type Void
      // 3.1 Function returns as void
      // There are various functions in C which do not return any value or
      // you can say they return void. A function with no return value has
      // the return type as void. For example, void exit (int status);

      // 3.2 Function arguments as void
      // There are various functions in C which do not accept any parameter.
      // A function with no parameter can accept a void. For example, int rand(void);

      // 3.3 Pointers to void
      // A pointer of type void * represents the address of an object, but not its type.
      // For example, a memory allocation function void *malloc( size_t size ); returns
      // a pointer to void which can be casted to any data type.

   // 4. Derived Types

   return 0;
}
