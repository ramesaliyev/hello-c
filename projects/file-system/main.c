#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define LINESIZE 256
#define VALUESIZE 80
#define NULCHR '\0'
#define NLCHR '\n'
#define NLSTR "\n"

/**
 * Tree Operations
 */

typedef struct tree Tree;
struct tree {
  char* value;
  Tree* parent;
  Tree* child;
  Tree* sibling;
};

Tree* malloc_tree() {
  Tree* tree = (Tree*) malloc(sizeof(Tree));
  tree->value = (char*) malloc(sizeof(char) * VALUESIZE);
  return tree;
}

Tree* create_tree(char* value) {
  Tree* tree = malloc_tree();

  strcpy(tree->value, value);
  tree->parent = NULL; 
  tree->child = NULL; 
  tree->sibling = NULL;

  return tree;
}

Tree* insert_child(Tree* root, char* value) {
  Tree* child = create_tree(value);
  child->parent = root;

  if (root->child == NULL) {
    root->child = child;
  } else {
    Tree* sibling = root->child;

    while (sibling->sibling != NULL) {
      sibling = sibling->sibling;
    }

    sibling->sibling = child;
  }

  return child;  
}

Tree* find_child(Tree* root, char* value) {
  if (root == NULL || root->child == NULL) {
    return NULL;
  }  

  Tree* child = root->child;

  while (child && strcmp(child->value, value) != 0) {
    child = child->sibling;
  }

  return child;
}

void remove_child(Tree* root, char* value) {
  Tree* child;
  Tree* current;

  if (strcmp(root->child->value, value) == 0) {
    child = root->child;
    root->child = child->sibling;
    free(child);
    return;
  }

  current = root->child;
  while (current->sibling && strcmp(current->sibling->value, value) != 0) {
    current = current->sibling;
  }

  if (current->sibling != NULL) {
    child = current->sibling;
    current->sibling = child->sibling;
    free(child);
  }
}

void free_tree(Tree* tree) {
  Tree* current = tree;
  Tree* next;
  
  while (current != NULL) {
    if (current->child) {
      free_tree(current->child);
    }

    next = current->sibling;

    free(current->value);
    free(current);

    current = next;
  }
}

void _print_tree(Tree* tree, int indent) {
  Tree* current = tree;
  
  while (current != NULL) {
    printf("%*s", indent, "");
    printf("%s\n", current->value);

    if (current->child) {
      _print_tree(current->child, indent + 2);
    }

    current = current->sibling;
  }
}

void print_tree(Tree* tree) {
  _print_tree(tree, 0);
}

/**
 * Common Operations
 */

bool inArrayChr(char* array, int size, char value) {
  int i = 0;
  while(i < size) if (array[i++] == value) return true;
  return false;
}

char* mallocstr(int size) {
  return (char*) malloc((size + 1) * sizeof(char));
}

void clearstr(char* str) {
  char allowed[] = {'-', ' ', '.'};
  size_t allowedlen = sizeof(allowed) / sizeof(char);

  int i, j;
  char chr;

  i = j = 0;
  while ((chr = str[i++]) != NULCHR) {
    if (isalnum(chr) || inArrayChr(allowed, allowedlen, chr)) {
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
 * Input Commands
 */
void mkdir(Tree* cwd, char* value) {
  if (value == NULL) return;

  Tree* child = find_child(cwd, value);

  if (child != NULL) {
    printf("Error: Subdirectory with name %s already exist.\n", value);
    return;
  }

  insert_child(cwd, value);
}

Tree* chdir(Tree* cwd, char* value) {
  if (value == NULL) return cwd;

  if (strcmp(value, "..") == 0) {
    if (cwd->parent == NULL) {
      printf("Error: Cannot go up, already in root.\n");
      return cwd;
    } else {
      return cwd->parent;
    }
  }

  Tree* child = find_child(cwd, value);

  if (child == NULL) {
    printf("Error: Working directory has no subdirectory named '%s'.\n\n", value);
    return cwd;
  }

  return child;
}

void rmdir(Tree* cwd, char* value) {
  if (value == NULL) return;

  Tree* child = find_child(cwd, value);

  if (child == NULL) {
    printf("Error: Working directory has no subdirectory named '%s'.\n\n", value);
  } else if (child->child != NULL) {
    printf("Error: Subdirectory '%s' cannot be deleted since it has children.\n\n", value);
  } else  {
    remove_child(cwd, value);
  }
}

void dir(Tree* cwd) {
  Tree* child = cwd->child;

  if (child == NULL) {
    printf("Directory is empty.\n\n");
  } else {
    while (child != NULL) {
      printf("%s\n", child->value);
      child = child->sibling;
    }
    
    printf("\n");
  }
}

void count(Tree* cwd) {
  Tree* child = cwd->child;
  int c = 0;

  while (child != NULL) {
    c++;
    child = child->sibling;
  }

  printf("%d\n\n", c);
}

void pwd(Tree* cwd) {
  Tree* path[256];
  int i = 0;

  Tree* current = cwd;
  while(current != NULL) {
    path[i++] = current;
    current = current->parent;
  }

  while(i--) {
    printf("/%s", path[i]->value);
  }

  printf("> ");
}

/**
 * Main Flow
 */
int main() {
  Tree* root = create_tree("root");
  Tree* cwd = root;

  printf("Welcome to YTUFileSystem 2021\n");
  printf("Available commands are listed bellow:\n");
  printf("  mkdir <name> - create new directory under current working directory. \n");
  printf("  chdir <name> - change current working directory, type '..' to go above. \n");
  printf("  rmdir <name> - remove given directory under current working directory \n");
  printf("  dir - list directories under current working directory \n");
  printf("  count - print count of directories under current working directory \n");
  printf("  ptree - print whole directory tree \n");
  printf("  exit - exit from program \n");
  printf("\n");

  while(true) {
    pwd(cwd);

    char* line = scanLine();
    
    if (strcmp(line, "") != 0) {
      char* cmd = strtok(line, " ");
      char* arg = strtok(NULL, " ");

      if (strcmp(cmd, "mkdir") == 0) {
        mkdir(cwd, arg);
      } else if (strcmp(cmd, "chdir") == 0) {
        cwd = chdir(cwd, arg);
      } else if (strcmp(cmd, "rmdir") == 0) {
        rmdir(cwd, arg);
      } else if (strcmp(cmd, "dir") == 0) {
        dir(cwd);
      } else if (strcmp(cmd, "count") == 0) {
        count(cwd);
      } else if (strcmp(cmd, "pwd") == 0) {
        pwd(cwd);
      } else if (strcmp(cmd, "ptree") == 0) {
        print_tree(root);
        printf("\n");
      } else if (strcmp(cmd, "exit") == 0) {
        break;
      } else {
        printf("Error: Unknown command '%s'\n", cmd);
      }
    }
  }

  free_tree(root);

  return 0;
}
