#include <stdio.h>
#include <string.h>

/*
   struct [structure tag] {
      member definition;
      ...
   } [one or more structure variables]; // Variables which types is this struct.
*/

// The structure tag is optional and each member definition is
// a normal variable definition, such as int i; or float f;
// or any other valid variable definition. At the end of the
// structure's definition, before the final semicolon, you can
// specify one or more structure variables but it is optional.

struct Books {
   char title[50];
   char author[50];
   char subject[100];
   int  book_id;
} book;

void printBook(struct Books book);

int main(int argc, char **argv) {
   struct Books Book1;
   struct Books Book2;

   strcpy(Book1.title, "C Programming");
   strcpy(Book1.author, "Nuha Ali");
   strcpy(Book1.subject, "C Programming Tutorial");
   Book1.book_id = 6495407;

   strcpy(Book2.title, "Telecom Billing");
   strcpy(Book2.author, "Zara Ali");
   strcpy(Book2.subject, "Telecom Billing Tutorial");
   Book2.book_id = 6495700;

   printBook(Book1);
   printBook(Book2);

   // To access the members of a structure using a pointer
   // to that structure, you must use the → operator as follows

   struct Books *bookPointer = &Book1;
   printf("Book pointer content title: %s\n", bookPointer->title);

   return 0;
}

void printBook(struct Books book) {
   printf("Book title: %s\n", book.title);
   printf("Book author: %s\n", book.author);
   printf("Book subject: %s\n", book.subject);
   printf("Book book_id: %d\n", book.book_id);
   printf("\n");
}