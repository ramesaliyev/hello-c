#include <stdio.h>

int main(int argc, char **argv) {

   int age;
   char gender;
   char name[256];

   printf("Enter age:\n");
   scanf("%d", &age);

   printf("Enter gender m/f:\n");
   scanf(" %c", &gender);

   printf("Enter name:\n");
   scanf("%s", name);

   printf("\n");
   printf("Age: %d\n", age);
   printf("Gender: %c\n", gender);
   printf("name: %s\n", name);

   return 0;
}
