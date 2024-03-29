#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/**
 * Stack Implementation START
 */
typedef struct stack_node {
  float data;
  struct stack_node *next;
} StackNode;

typedef struct stack {
  StackNode * top;
  int size;
  int counter;
} Stack;

// Memory allocation for stack node.
StackNode * malloc_stack_node() {
  return (StackNode *) malloc(sizeof(StackNode));
}

// Memory allocation for stack.
Stack * malloc_stack() {
  return (Stack *) malloc(sizeof(Stack));
}

// Create stack with size.
// Pass -1 for unlimited stack.
Stack * create_stack(int size) {
  Stack * new_stack = malloc_stack();
  new_stack->top = NULL;
  new_stack->size = size;
  new_stack->counter = 0;
  return new_stack;
}

// Check if stack is empty.
bool is_stack_empty(Stack * stack) {
  return stack->top == NULL;
}

// Check if stack is full.
bool is_stack_full(Stack * stack) {
  return stack->counter == stack->size;
}

// Peek top item of stack without popping it.
float peek_stack(Stack * stack) {
  if (!is_stack_empty(stack)) {
    return stack->top->data;
  }

  return 0;
}

// Pop top item of stack.
float pop_stack(Stack * stack) {
  if (!is_stack_empty(stack)) {
    StackNode * top = stack->top;
    stack->top = top->next;
    stack->counter--;

    float data = top->data;
    free(top);
    return data;
  }

  return 0;
}

// Push item on top of the stack.
void push_stack(Stack * stack, float data) {
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

// Destroy stack.
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

// Print whole stack.
void print_stack(Stack *stack) {
  StackNode * current_node = stack->top;

  if (current_node == NULL) {
    printf("Stack is empty.\n");
  } else {
    while (current_node != NULL) {
      printf("%.2f\n", current_node->data);
      current_node = current_node->next;
    }
  }
}
/**
 * Stack Implementation END
 */

/**
 * Utils implementation START
 */
// Remove empty spaces from given string.
const char * remove_empty_spaces(const char * string) {
  char * new_string = (char *) calloc(strlen(string), sizeof(char));

  char * string_cursor = (char *) string;
  char * new_string_cursor = new_string;

  char current_char;
  while ((current_char = *(string_cursor)) != '\0') {
    if (current_char != ' ') {
      *(new_string_cursor) = current_char;
      new_string_cursor++;
    }

    string_cursor++;
  }

  *(new_string_cursor) = '\0';

  return new_string;
}

// Get program input string at initialization.
const char * get_program_input_string(int argc, char **argv) {
  char *input_string;

  if (argc < 2) {
    return NULL;
  }

  int input_size = 0;

  int i;
  for (i = 1; i < argc; i++) {
    input_size += strlen(argv[i]);
    if (i + 1 < argc) {
      input_size++;
    }
  }

  input_string = (char *) malloc(input_size);

  // Memory allocation is failed.
  if (input_string == NULL) {
    return NULL;
  }

  input_string[0] = '\0';

  int j;
  for (j = 1; j < argc; j++) {
    strcat(input_string, argv[j]);
    if (j + 1 < argc) {
      strcat(input_string, " ");
    }
  }

  return input_string;
}

// Ask user to input something, and get whole as string.
const char * prompt_user_input_string(const char * prompt_string) {
  printf("%s ", prompt_string);

  int max_char = 10;
  char * input_string = (char *) malloc(max_char);
  char * input_string_root = input_string;
  int remained_char = max_char;
  int current_char;

  // Memory allocation is failed.
  if(input_string == NULL) {
    return NULL;
  }

  while(true) {
    current_char = fgetc(stdin);
    remained_char--;

    if(remained_char == 0) {
      remained_char = max_char;
      max_char *= 2;
      char * new_input_string = (char *) realloc(input_string_root, max_char);

      // Memory allocation is failed.
      if (new_input_string == NULL) {
          free(input_string_root);
          return NULL;
      }

      input_string = new_input_string + (input_string - input_string_root);
      input_string_root = new_input_string;
    }

    if (current_char == '\n') {
      break;
    }

    *(input_string++) = current_char;
  }

  *(input_string) = '\0';

  return input_string_root;
}
/**
 * Utils implementation END
 */

/**
 * Arithmetic Calculator implementation START
 */
enum symbol {
  DOT = -10,
  PAREN_OPEN = -100,
  PAREN_CLOSE = -101,
  MULTIPLY = -200,
  DIVIDE = -201,
  ADD = -300,
  SUBTRACT = -301
};

// Convert given token into known symbol.
int get_symbol(char token) {
  switch (token) {
    case '(': return PAREN_OPEN;
    case ')': return PAREN_CLOSE;
    case '*': return MULTIPLY;
    case '/': return DIVIDE;
    case '+': return ADD;
    case '-': return SUBTRACT;
    case '.': return DOT;
    default : return token - '0'; // convert to integer.
  }
}

// Convert given known symbol into char.
// Works only for known symbols, wont work for integers.
char get_symbol_itself(int symbol) {
  switch (symbol) {
    case DOT: return '.';
    case PAREN_OPEN: return '(';
    case PAREN_CLOSE: return ')';
    case MULTIPLY: return '*';
    case DIVIDE: return '/';
    case ADD: return '+';
    case SUBTRACT: return '-';
    default : return ' ';
  }
}

// Check given symbols equality, Returns:
//  0 if they equal,
//  1 if symbol_a > symbol_b
// -1 if symbol_a < symbol_b
int get_symbol_equality(int symbol_a, int symbol_b) {
  int state = symbol_a - symbol_b;
  if (abs(state) == 1) return 0;
  if (state > 0) return 1;
  return -1;
}

// Parse and convert a string into meaningful tokens.
float * parse_tokens(const char * string) {
  int max_decimal_digits = 100;
  int max_token = 20;

  float * tokens = (float *) calloc(max_token, sizeof(float));
  float * tokens_cursor = tokens;
  int remained_token = max_token;

  if (tokens == NULL) {
    return NULL;
  }

  char * string_cursor = (char *) string;

  // Maximum allowed number of decimal digits is 100.
  char * current_token = (char *) calloc(max_decimal_digits, sizeof(char));
  char * current_token_cursor = current_token;

  if (current_token == NULL) {
    return NULL;
  }

  char current_char;
  while ((current_char = *(string_cursor)) != '\0') {
    float current_symbol = get_symbol(current_char);

    if (remained_token <= 2) {
      remained_token = max_token;
      max_token *= 2;
      float * new_tokens = (float *) realloc(tokens_cursor, max_token);

      if (new_tokens == NULL) {
        free(tokens);
        return NULL;
      }

      tokens = new_tokens + (tokens - tokens_cursor);
      tokens_cursor = new_tokens;
    }

    // Being greater than 0 means its not an operator.
    // Add next to current token.
    if (current_symbol >= 0) {
      *(current_token_cursor) = current_symbol + '0'; // convert to char
      current_token_cursor++;
    } else if (current_symbol == DOT) { // dot
      *(current_token_cursor) = get_symbol_itself(DOT); // convert to char
      current_token_cursor++;
    } else {
      // First convert current_token to int and add tokens (if not empty).
      if (current_token != current_token_cursor) {
        *(tokens) = atof(current_token);
        tokens++;
        remained_token--;

        // Reset current token pointer.
        free(current_token);
        current_token = (char *) calloc(max_decimal_digits, sizeof(char));
        current_token_cursor = current_token;

        if (current_token == NULL) {
          free(tokens);
          return NULL;
        }
      }

      *(tokens) = current_symbol;
      tokens++;
      remained_token--;
    }

    string_cursor++;
  }

  if (current_token != current_token_cursor) {
    *(tokens) = atof(current_token);
    tokens++;
  }

  *(tokens) = -1; // end of tokens.

  return tokens_cursor;
}

// Print stacks.
void print_token_stack(const char * name, Stack * stack) {
  printf("%s Stack: ", name);

  StackNode * current_node = stack->top;

  if (current_node == NULL) {
    printf("Empty.");
  } else {
    while (current_node != NULL) {
      float token = current_node->data;
      token >= 0 ? printf("%.2f ", token) : printf("%c ", get_symbol_itself(token));
      current_node = current_node->next;
    }
  }
  printf("\n");
}

// Print tokens.
void print_tokens(float * tokens) {
  printf("Parsed Tokens: ");
  float * tokens_print_cursor = tokens;
  while(*tokens_print_cursor != -1) {
    float token = *tokens_print_cursor;
    token >= 0 ? printf("%.2f ", token) : printf("%c ", get_symbol_itself(token));
    tokens_print_cursor++;
  }
  printf("\n");
}

// Apply an operator to two numbers.
float apply_operator(int operator, float b, float a, bool log) {
  if (log) {
    printf("Calculating: %.2f %c %.2f \n", a, get_symbol_itself(operator), b);
  }

  switch (operator) {
    case MULTIPLY: return a * b;
    case DIVIDE: return a / b;
    case ADD: return a + b;
    case SUBTRACT: return a - b;
    default : return 0; // wont happen.
  }
}

// Do arithmetic calculation over a given string.
float calculate(const char * string_input, bool log) {
  float * tokens = parse_tokens(remove_empty_spaces(string_input));
  float * tokens_cursor = tokens;

  if (log) {
    print_tokens(tokens);
    printf("-\n");
  }

  Stack * operators = create_stack(-1);
  Stack * operands = create_stack(-1);

  // End of tokens marked as -1.
  while (*tokens_cursor != -1) {
    float current_token = *tokens_cursor;

    if (log) {
      print_token_stack("Operands", operands);
      print_token_stack("Operators", operators);
      printf("-\n");
    }

    // Token is an operator.
    switch ((int) current_token) {
      case PAREN_OPEN: {
        push_stack(operators, current_token);
        break;
      }

      case PAREN_CLOSE: {
        int operator;
        while((operator = pop_stack(operators)) != PAREN_OPEN) {
          float num_b = pop_stack(operands);
          float num_a = pop_stack(operands);
          float result = apply_operator(operator, num_b, num_a, log);
          push_stack(operands, result);
        }

        break;
      }

      case MULTIPLY:
      case DIVIDE:
      case ADD:
      case SUBTRACT: {
        // While priority of peak operator is
        // greater than or equal to current operator.
        while (
          !is_stack_empty(operators) && peek_stack(operators) != PAREN_OPEN &&
          get_symbol_equality(peek_stack(operators), current_token) >= 0
        ) {
          float num_b = pop_stack(operands);
          float num_a = pop_stack(operands);
          float operator = pop_stack(operators);
          float result = apply_operator(operator, num_b, num_a, log);
          push_stack(operands, result);

          if (log) {
            print_token_stack("Operands", operands);
            print_token_stack("Operators", operators);
            printf("-\n");
          }
        }

        push_stack(operators, current_token);
        break;
      }

      // Token is an integer.
      default: {
        push_stack(operands, current_token);
        break;
      }
    }

    tokens_cursor++;
  }

  if (log) {
    print_token_stack("Operands", operands);
    print_token_stack("Operators", operators);
    printf("-\n");
  }

  while (!is_stack_empty(operators)) {
    float num_b = pop_stack(operands);
    float num_a = pop_stack(operands);
    float operator = pop_stack(operators);
    float result = apply_operator(operator, num_b, num_a, log);
    push_stack(operands, result);

    if (log) {
      print_token_stack("Operands", operands);
      print_token_stack("Operators", operators);
      printf("-\n");
    }
  }

  float result = pop_stack(operands);

  destroy_stack(operands);
  destroy_stack(operators);

  return result;
}
/**
 * Arithmetic Calculator implementation END
 */

/**
 * Program implementation START
 */
int program(const char * input_string) {
  while (true) {
    if (input_string != NULL) {
      printf("User Input: %s\n", input_string);
      printf("Result: %.2f\n", calculate(input_string, true));
      printf("\n");
    }
    input_string = prompt_user_input_string("Enter new math expression:");
  }

  return 0;
}

int test() {
  const char * expressions[5] = {
    "12 * (4 + 3 - 16 / 8) - 5",
    "8 + 2 * (21 / (7 - 4) + 2)",
    "5 + 3 * 10 / 6 - 2",
    "(12 + 4 - 3) * (7 * 2 + 5)",
    "21 / ((4 + 8) * 2 - 17)"
  };

  int results[5] = {55, 26, 8, 247, 3};

  int i;
  for (i = 0; i < 5; i++) {
    const char * expression = expressions[i];
    int correct_result = results[i];
    float result = calculate(expression, false);

    printf(
      "%-30s should be equal %-10d result is %-10.2f -> %s \n",
      expression, correct_result, result,
      result == correct_result ? "success" : "error"
    );
  }

  return 0;
}
/**
 * Program implementation END
 */

/**
 * Main program input.
 */
int main(int argc, char **argv) {
  const char * input_string = get_program_input_string(argc, argv);

  return input_string != NULL && strcmp(input_string, "test") == 0 ?
    test() : program(input_string);
}
