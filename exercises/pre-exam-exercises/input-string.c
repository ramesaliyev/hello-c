#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "math.h"

typedef struct Text {
  int length;
  int index;
  char* text;
} Text;

Text* create_text() {
  Text* text = (Text*) malloc(sizeof(Text));
  text->index = 0;
  text->length = 8;
  
  text->text = (char*) calloc(text->length, sizeof(char));
  *(text->text) = '\0';
  
  return text;  
}

void extend_text(Text* text) {
  if (text->index == text->length - 1) {
    text->length *= 2;
    text->text = (char*) realloc(text->text, text->length);
  }
}

void append_text(Text* text, char key) {
  extend_text(text);

  *(text->text + text->index) = key;
  *(text->text + text->index + 1) = '\0';
  text->index++;
}

void print_text(Text* text) {
  printf("Text: %s\n", text->text);
}

void print_meta(Text* text) {
  print_text(text);
  printf("Length: %d, Index:%d\n", text->length, text->index);
}

void input(Text* text) {
  char key;
  
  while (true) {
    key = fgetc(stdin);
    
    if (key == '\n') {
      break;
    }

    append_text(text, key);
  }
}

int main() {
  Text* text = create_text();

  printf("Enter endless dynamic string: \n");
  while (true) {
    input(text);
    print_meta(text);
  }

  free(text);

  return 0;
}
