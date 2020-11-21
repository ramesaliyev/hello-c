#include "stdio.h"

int main() {
  char str[] = "whatssup dude";
  char* p = str; // will decay

  while (*p != '\0') {
    printf("%c\n", *p++);
  }

  return 1;
}
