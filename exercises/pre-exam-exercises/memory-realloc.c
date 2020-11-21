#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"

#define DEFAULT_SIZE 2

typedef struct Store {
  int size;
  int index;
  char* data;
} Store;

Store* create_store() {
  Store* store = (Store *) malloc(sizeof(Store));
  store->size = DEFAULT_SIZE;
  store->index = 0;
  store->data = (char *) calloc(DEFAULT_SIZE, sizeof(char));
  
  return store;
}

void resize_store(Store* store) { 
  // upsize
  if (store->index == store->size - 1) {
    store->size = store->size * 2;
    store->data = (char *) realloc(store->data, sizeof(char) * store->size);
  }
}

void add_store(Store* store, char val) {
  resize_store(store);

  *(store->data + store->index) = val;
  store->index++;
}

void input(Store* store) {
  printf("Enter as many chars as you want, enter 0 to exit.\n");
  char key;

  while (true) {
    scanf(" %c", &key);
    if (key == '0') return;
    add_store(store, key);
  }
}

void list(Store* store) {
  int i;
  for (i = 0; i < store->size; i++) {
    printf("%c ", *(store->data + i));
  }
  printf("\n");
}

void meta(Store* store) {
  printf("Store info: size:%d, index:%d\n", store->size, store->index);
}

int main() {
  Store* store = create_store();

  while(true) {
    char command;

    printf("enter command. 0=exit, 1=input, 2=list, 3=meta\n");
    scanf(" %c", &command);

    if (command == '0') return 1;
    if (command == '1') input(store);
    if (command == '2') list(store);
    if (command == '3') meta(store);

    printf("\n");
  }

  return 1;
}
