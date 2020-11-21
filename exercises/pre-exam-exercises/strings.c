#include "stdio.h"
#include "string.h"

int main() {
  char str1[] = "whatsup";
  char str2[strlen(str1)];

  strcpy(str2, str1);

  printf("%s\n", str1);
  printf("%s\n", str2);

  return 1;
}
