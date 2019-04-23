#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_LINE_LENGTH 1000
#define MAX_FRIEND_COUNT 20

typedef struct Users Users;
typedef struct User User;

char * prompt_user_input_string(const char * prompt_string);
void importUsers(Users * users);
void insertNewUserByImportString(Users * users, char line[MAX_LINE_LENGTH]);
void insertNewUser(Users * users, int id, const char * fullname, int * friends);

struct Users {
  User * user;
};

struct User {
  int id;
  const char * fullname;
  int friends[MAX_FRIEND_COUNT];
  User * left;
  User * right;
};

Users * mallocUsers() {
  return (Users *) malloc(sizeof(Users));
}

User * mallocUser() {
  return (User *) malloc(sizeof(User));
}

// Create a new users type.
Users * createUsers() {
  Users * users = mallocUsers();
  users->user = NULL;
  return users;
}

// Create a new user type.
User * createUser(int id, const char * fullname, int * friends) {
  User * user = mallocUser();
  user->id = id;
  user->fullname = fullname;
  user->left = NULL;
  user->right = NULL;

  int i = 0;
  while(*friends != -1) {
    user->friends[i] = *friends;
    friends++;
    i++;
  }

  return user;
}

// Insert user into BST.
User * insertUserToTree(User * parent, User * user) {
  if (parent == NULL) return user;

  if (user->id < parent->id) {
    parent->left = insertUserToTree(parent->left, user);
  } else if (user->id > parent->id) {
    parent->right = insertUserToTree(parent->right, user);
  }

  return parent;
}

User * userWithMinId(User * user) {
  User * current = user;

  while (current->left != NULL) {
    current = current->left;
  }

  return current;
}

// Import users from a file.
void importUsers(Users * users) {
  // const char * filePath = prompt_user_input_string("Enter the file path:");
  const char * filePath = "problems/social-network-in-bst/users.txt";
  printf("%s", filePath);

  FILE * file;
  char line[MAX_LINE_LENGTH];
  file = fopen(filePath, "r");

  if (file == NULL){
    printf("Could not open file '%s' \n", filePath);
    return;
  }

  printf("\n\nImporting users...\n\n");

  int count = 0;
  while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
    insertNewUserByImportString(users, line);
    count++;
  }

  printf("Import completed. %d users imported.\n", count);

  fclose(file);
}

void insertNewUserByImportString(Users * users, char * importString) {
  char string[MAX_LINE_LENGTH];
  int j = 0;
  while(*importString != '\0') string[j++] = *(importString++);

  string[strcspn(string, "\r\n")] = 0;
  printf("Import String: <%s>\n", string);

  char * args[3] = {NULL, NULL, NULL};
  int i = 0;
  args[i] = strtok(string, ",");

  while(args[i] != NULL) {
    args[++i] = strtok(NULL, ",");
  }

  int id = atoi(args[0]);
  char * name = args[1];
  int * friendIds = (int *) calloc(MAX_FRIEND_COUNT, sizeof(int));
  int * friendIds_cursor = friendIds;

  printf("Parsed Import Tokens: (id:%d) ", id);
  printf("(name:%s) ", name);

  if (args[2] != NULL) {
    printf("(friends:");

    char * friends[MAX_FRIEND_COUNT] = {""};
    int j = 0;
    friends[j] = strtok(args[2], "-");
    printf("%s", friends[j]);

    while(friends[j] != NULL) {
      char * friendId = strtok(NULL, "-");
      if (friendId != NULL) {
        friends[++j] = friendId;
        printf(",%s", friends[j]);
      } else {
        break;
      }
    }

    int k;
    for (k = 0; k <= j; k++) *(friendIds++) = atoi(friends[k]);
    printf(")");
  }

  *(friendIds) = -1;

  insertNewUser(users, id, name, friendIds_cursor);
  printf("\n");
}

// Insert a new user into bst.
void insertNewUser(Users * users, int id, const char * fullname, int * friends) {
  printf("\nInserting new user: (id:%d) (name:%s)", id, fullname);
  int i = 0;
  if (*friends != -1) {
    printf(" (friends: %d", *friends);
    while(*(++friends) != -1) printf(",%d", *friends);
    printf(")");
  }

  User * newUser = createUser(id, fullname, friends);
  users->user = insertUserToTree(users->user, newUser);

  printf("\n");
}

// Delete an user from bst.
void deleteUser(Users * users, int id) {

}

// Check if user exist in bst.
// Print fullname if does.
void contains(Users * users, int id) {

}

// Find user by id and print names of friends.
void friends(Users * users, int id) {

}

// Print names of children nodes of user node by id.
// In ascending order.
void printNext(Users * users, int id) {

}

// Print ids and names of users which id bigger than given id.
void printGreater(Users * users, int id) {

}

// Print all users in ascending order by id.
void printInOrder(Users * users) {

}

// Print with given indent.
void print_with_indent(int indent, char * string) {
  printf("%*s%s", indent, "", string);
}

// Print user tree.
void printUserTree(User * user, int indent, char * s) {
  if (user == NULL) return;

  if (indent == 0) {
    printf("\nTree START");
  } else {
    printf("\n | ");
  }

  print_with_indent(indent, "");
  printf("%s\n | ", s);
  print_with_indent(indent, "");
  printf("(%d)", user->id);

  printUserTree(user->left, indent + 4, "/");
  printUserTree(user->right, indent + 4, "\\");

  if (indent == 0) printf("\nTree END\n");
};

// Get node count of users.
int size(Users * users) {
  return 0;
}

// Ask user to input something, and get whole as string.
char * prompt_user_input_string(const char * prompt_string) {
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

// Main function.
int main() {
  Users * users = createUsers();
  char action = '0';

  while(action != 'q') {
    if (action == '0') {
      printf("Welcome to Social Network implementation in BST.\n");
      printf("Choose an action to run. Enter the letter on the left of the list to perform.\n");
    }

    if (action == '0' || action == '1') {
      printf("\n===Actions================================================\n");
      printf("(a) insertNewUser: Insert a new user with given values.\n");
      printf("(b) deleteUser: Delete an user by id.\n");
      printf("(c) contains: Check if user exist, print fullname if it does.\n");
      printf("(d) friends: Find user by id and print names of its friends.\n");
      printf("(e) size: Get total count of users.\n");
      printf("(f) printNext: Print names of children nodes of user in ascending order.\n");
      printf("(g) printGreater: Print ids and names of users which id bigger than given id.\n");
      printf("(h) printInOrder: Print all users in ascending order by id.\n");
      printf("\n");
      printf("(i) importUsers: Import users from a file.\n");
      printf("(j) printUserTree: Prints users tree in graphical matter.\n");
      printf("\n");
      printf("(q) quit/exit: End the program and exit.\n");
      printf("============================================================\n");
    }

    action = prompt_user_input_string("\nPress a key from the list: ")[0];

    switch (action) {
      case 'a':
        printf("\n-> Insert a new user with given values.\n");
        char * id = prompt_user_input_string("\nEnter id: ");
        char * fullname = prompt_user_input_string("Enter name and surname: ");
        char * friends = prompt_user_input_string("Enter friend ids (dash separated like 1-2-3): ");

        char * input = (char *) malloc(strlen(id) + strlen(fullname) + strlen(friends) + 2);
        strcpy(input, id);
        strcat(input, ",");
        strcat(input, fullname);
        strcat(input, ",");
        strcat(input, friends);

        insertNewUserByImportString(users, input);

        break;
      case 'b':
        printf("\n-> Delete an user by id.\n\n");
        break;
      case 'c':
        printf("\n-> Check if user exist, print fullname if it does.\n\n");
        break;
      case 'd':
        printf("\n-> Find user by id and print names of its friends.\n\n");
        break;
      case 'e':
        printf("\n-> Get total count of users.\n\n");
        break;
      case 'f':
        printf("\n-> Print names of children nodes of user in ascending order.\n\n");
        break;
      case 'g':
        printf("\n-> Print ids and names of users which id bigger than given id.\n\n");
        break;
      case 'h':
        printf("\n-> Print all users in ascending order by id.\n\n");
        break;
      case 'i':
        printf("\n-> Import users from a file.\n\n");
        importUsers(users);
        break;
      case 'j':
        printf("\n-> Prints users tree in graphical matter.\n\n");
        printUserTree(users->user, 0, "");
        break;
      case 'q':
        printf("\n-> Quit.\n\n");
        return 0;
      default:
        printf("\n-> Unknown action choosed, try again.\n");
        continue;
    }

    action = '1';
  }

  return 0;
}