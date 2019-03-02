# Compile & Run

#### Compile

    $ cc name1.c name2.c -o nameX

#### Run

    $ ./nameX

# Memory

#### Memory Size of Types

|Common term|C type|Number of bits|Max unsigned|
|:---             |:---|:---|:---|
|Byte             |char|8|255|
|Word             |short|16|65535|
|Integer          |int|32|~4 billion|
|Long Integer     |long|32 / 64|~4 billion / ~18 qunitillion|
|Long Long Integer|long long|64|~18 qunitillion|
|Float            |float|32|3.402823466 e+38|
|Double Float     |double|64|1.7976931348623158 e+308|
|Pointer          |* X|32 / 64|~4 billion / ~18 qunitillion|

- **Pointers** are just integers. For 32 bit architectures pointers are 32 bits in size, and are 64 bits in size on 64 bit architectures.

- When we want **signed** numbers, we center our ranges AROUND 0. So bytes (chars) can go from -128 to 127, shorts from -32768 to 32767, and so on.

- **By default all of the types are signed** (except pointers) UNLESS you put an “unsigned” in front of them. You can also place “signed” in front to explicitly say you want the type to be signed.

- Float and Double types cannot be signed or unsigned.

- Memory to a machine is just a big “spreadsheet” of numbers. Imagine it as a spreadsheet with only 1 column and a lot of rows. Every cell can store 8 bits (a byte). If you “merge” rows (2, 4, 8) you can store more values as above. But when you merge rows, the next row number doesn't change. You also could still address the “parts” of the merged cell as bytes or smaller units. In the end pointers are nothing more than a number saying “go to memory row 2943298 and get me the integer (4 bytes) located there” (if it was a pointer to an integer). The pointer itself just stores the PLACE in memory where you find the data. The data itself is what you get when you de-reference a pointer.

- You can have a pointer to pointers, so de-reference a pointer to pointers to get the place in memory where the actual data is then de-reference that again to get the data itself. Follow the chain of pointers if you want values. Since pointers are numbers, you can do math on them like any other. You can advance through memory just by adding 1, 2, 4 or 8 to your pointer to point to the “next thing along” for example, which is how arrays work.

- Variables are stored on the **stack**. This is a special place in memory for such temporary variables and data. It grows and shrinks as needed and has a specific order to store data. Other data is likely on the **“heap”** and you will explicitly ask it to be there. The heap is basically a “big mess of data” where you likely have more permanent data or the larger bits of data. C provides some basic methods to ask for this storage, where the stack allocation space is generally implicit when you call a function.

- If you don't initialize a variable with initial value, its value may be random garbage that was there before in memory.

- Arrays have strict ordering in memory, so you can later on use pointers and simple arithmetic to walk up and down an array to access data.

# Pointers

- If you have a variable var in your program, *&var* will give you its address in the memory, where **&** is commonly called the **reference operator**.

- You can also create a special variable that stores the address (rather than the value). This variable is called **pointer variable** or simply a **pointer**.

      // syntax
      data_type* pointer_variable_name;

      // example
      int* p;

- Like reference operator, there is another operator "**\***" that gets you the value from the address, it is called a **dereference operator** \*.

- The * sign when declaring a pointer is not a dereference operator. It is just a similar notation that creates a pointer.

- Pointer itself is also a variable in memory. So you could also get address of the pointer variable. And even declare another pointer which points to the this pointer. Pointerception.

- **Pointer Arithmetics** A limited set of arithmetic operations can be performed on pointers. A pointer may be: incremented (++), decremented (--), an integer may be added to a pointer (+ or +=), an integer may be subtracted from a pointer (– or -=).

- The **pointer arithmetic is performed relative to the base type of the pointer**. For example, if we have an integer pointer ip which contains address 1000, then on incrementing it by 1, we will get 1004 (i.e 1000 + 1 * 4) instead of 1001 because the size of the int data type is 4 bytes. If we had been using a system where the size of int is 2 bytes then we would get 1002 ( i.e 1000 + 1 * 2 ).

- Always C pointer is initialized to null, i.e. `int *p = null`.

- The value of null pointer is 0.

- If a pointer in C is assigned to NULL, it means it is pointing to nothing.

- Two pointers can be subtracted to know how many elements are available between these two pointers.

- **Common pointer mistakes:**.

      int c, *pc;

      // Wrong! pc is address whereas,
      // c is not an address.
      pc = c;

      // Wrong! *pc is the value pointed by address whereas,
      // &c is an address.
      *pc = &c;

      // Correct! pc is an address and,
      // &c is also an address.
      pc = &c;

      // Correct! *pc is the value pointed by address and,
      // c is also a value (not address).
      *pc = c;

See [basics/pointers.c](basics/pointers.c) for examples.

# Storage Classes
A storage class defines the scope (visibility) and life-time of variables and/or functions within a C Program. They precede the type that they modify. We have four different storage classes in a C program

- **auto**
  The auto storage class is the default storage class for all local variables. 'auto' can only be used within functions, i.e., local variables.

- **register**
  The register storage class is used to define local variables that should be stored in a register instead of RAM. This means that the variable has a maximum size equal to the register size (usually one word) and can't have the unary '&' operator applied to it (as it does not have a memory location). The register should only be used for variables that require quick access such as counters. It should also be noted that defining 'register' does not mean that the variable will be stored in a register. It means that it MIGHT be stored in a register depending on hardware and implementation restrictions.

- **static**
  The static storage class instructs the compiler to keep a local variable in existence during the life-time of the program instead of creating and destroying it each time it comes into and goes out of scope. Therefore, making local variables static allows them to maintain their values between function calls. The static modifier may also be applied to global variables. When this is done, it causes that variable's scope to be restricted to the file in which it is declared. In C programming, when static is used on a global variable, it causes only one copy of that member to be shared by all the objects of its class.

- **extern**
  The extern storage class is used to give a reference of a global variable that is visible to ALL the program files. When you use 'extern', the variable cannot be initialized however, it points the variable name at a storage location that has been previously defined. When you have multiple files and you define a global variable or function, which will also be used in other files, then extern will be used in another file to provide the reference of defined variable or function. Just for understanding, extern is used to declare a global variable or function in another file.

# Resources

- https://www.geeksforgeeks.org/c-programming-language/
- https://www.enlightenment.org/docs/c/start
- https://www.tutorialspoint.com/cprogramming/index.htm
- https://www.programiz.com/c-programming/c-pointers
- https://overiq.com/c-programming-101/pointer-arithmetic-in-c/
- https://fresh2refresh.com/c-programming/c-pointer/
