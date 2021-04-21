#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define LINESIZE 256
#define NULCHR '\0'
#define NLCHR '\n'
#define NLSTR "\n"

/**
 * Stack Implementation 
 */
typedef struct link {
  int data;
  struct link *next;
} StackNode;

typedef struct stackLinkedList {
  StackNode * top;
  int size;
  int counter;
} Stack;

StackNode * malloc_stack_node() {
  return (StackNode *) malloc(sizeof(StackNode));
}

Stack * malloc_stack() {
  return (Stack *) malloc(sizeof(Stack));
}

Stack * create_stack(int size) {
  Stack * new_stack = malloc_stack();
  new_stack->top = NULL;
  new_stack->size = size;
  new_stack->counter = 0;
  return new_stack;
}

bool is_stack_empty(Stack * stack) {
  return stack->top == NULL;
}

bool is_stack_full(Stack * stack) {
  return stack->counter == stack->size;
}

int peek_stack(Stack * stack) {
  if (!is_stack_empty(stack)) {
    return stack->top->data;
  }

  return 0;
}

int pop_stack(Stack * stack) {
  if (!is_stack_empty(stack)) {
    StackNode * top = stack->top;
    stack->top = top->next;
    stack->counter--;

    int data = top->data;
    free(top);
    return data;
  }

  return 0;
}

void push_stack(Stack * stack, int data) {
  if (!is_stack_full(stack)) {
    StackNode * new_node = malloc_stack_node();
    new_node->next = NULL;
    new_node->data = data;

    if (stack->top != NULL) {
      new_node->next = stack->top;
    }

    stack->top = new_node;
    stack->counter++;
  }
}

void destroy_stack(Stack * stack) {
  if (!is_stack_empty(stack)) {
    StackNode * node = stack->top;

    while (node->next != NULL) {
      StackNode * tmp = node;
      node = tmp->next;
      free(tmp);
    }

    free(node);
  }

  free(stack);
}

void print_stack(Stack *stack) {
  StackNode * current_node = stack->top;

  printf("\n");
  while (current_node != NULL) {
    printf("%d\n", current_node->data);
    current_node = current_node->next;
  }
  printf("\n");
}

/**
 * Common Operations
 */
char* mallocstr(int size) {
  return (char*) malloc((size + 1) * sizeof(char));
}

char* copystr(char* str) {
  char* copy = mallocstr(strlen(str));
  if (copy) strcpy(copy, str);
  return copy;
}

void clearstr(char* str) {
  int i, j;
  char chr;

  i = j = 0;
  while ((chr = str[i++]) != NULCHR) {
    if (isalnum(chr)) {
      str[j++] = chr;
    }
  }

  str[j] = NULCHR;
}

char* scanRaw(int size) {
  char* input = mallocstr(size);
  fgets(input, size, stdin);

  if (input[strlen(input)-1] != NLCHR) {
    int ch;
    while (((ch = getchar()) != NLCHR) && (ch != EOF));
  }

  return input;
}

char* scan(int size) {
  char* input = scanRaw(size);
  clearstr(input);
  return input;
}

char* scanStr(int size) {
  char* input = scanRaw(size);

  if (strcmp(input, NLSTR) == 0) {
    strcpy(input, "");
  } else if (strcmp(input, " \n") == 0) {
    strcpy(input, "");
  } else {
    clearstr(input);
  }

  return input;
}

char* scanLine() {
  return scanStr(LINESIZE);
}

/**
 * Problem Resolution
 */
char* decodeSpyCode(char* code) {
  Stack * stack = create_stack(LINESIZE);
  
  int i = 0;
  bool error = false;
  while (!error && code[i] != NULCHR) {
    char token = code[i];

    if (isalpha(token)) {
      push_stack(stack, token);
    } else if (isdigit(token)) {
      int popCount = token - '0'; // to int
      
      if (popCount > stack->counter) {
        error = true;
      } else {
        int j = 0;
        while (j < popCount) {
          pop_stack(stack);
          j++;
        }
      }
    }

    i++;
  }

  if (error) {
    destroy_stack(stack);
    return NULL;
  }

  // create string.
  char* decodedCode = mallocstr(LINESIZE);
  decodedCode[stack->counter] = NULCHR;
  while (stack->counter > 0) {
    decodedCode[stack->counter] = pop_stack(stack);
  }

  destroy_stack(stack);

  return decodedCode;
}

/**
 * Main Flow
 */ 
int main() {
  printf("Welcome to SpyEnigma2021.\n");
  printf("Enter code of Spy1: ");
  char* codeSpy1 = scanLine();

  printf("Enter code of Spy2: ");
  char* codeSpy2 = scanLine();

  // Example entries;
  // char* codeSpy1 = "cabc2ssss3uvxyz4sab2kabak4odun1";
  // char* codeSpy2 = "ama3cam1ses2usta2kopuk3yok3damla4ufuk3";

  char* decodedCode1 = decodeSpyCode(codeSpy1);
  char* decodedCode2 = decodeSpyCode(codeSpy2);

  if (decodedCode1 == NULL) {
    printf("\nFormat of Spy1's code is wrong: %s \n", codeSpy1);
  } else if (decodedCode2 == NULL) {
    printf("\nFormat of Spy2's code is wrong: %s \n", codeSpy2);
  } else {
    printf("\n");
    printf("Code of Spy1: encoded=[%s] decoded=[%s] \n", codeSpy1, decodedCode1);
    printf("Code of Spy2: encoded=[%s] decoded=[%s] \n", codeSpy2, decodedCode2);

    if (strcmp(decodedCode1, decodedCode2) == 0) {
      printf("\nCodes of Spies are matched! Proceed to world domination. \n");
    } else {
      printf("\nCodes of Spies are NOT MATCHED! Run away this is a TRAP!. \n");
    }
  }

  free(codeSpy1);
  free(codeSpy2);
  
  if (decodedCode1 != NULL) {
    free(decodedCode1);
  }

  if (decodedCode2 != NULL) {
    free(decodedCode2);
  }

  return 0;
}
