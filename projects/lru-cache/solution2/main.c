#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/**
 * README
 * 
 * Please start reading code from Main (Entry Point), 
 * and go upwards to easily follow solution flow.
 * 
 * Code structure:
 * Search like "(x)" to jump directly to section.
 * 
 * (1) Program configuration definitions.
 * (2) Data types and structures.
 * (3) Generic helper functions.
 * (4) Problem solution.
 * (5) File processing and output.
 * (6) Main (Entry Point)
 * 
 */

/**
 * (1) Program configuration definitions.
 */
#define LINESIZE 256

/**
 * (2) Data types and structures.
 */
typedef struct LRUCache LRUCache;
typedef struct HashTable HashTable;
typedef struct HashRow HashRow;
typedef struct ListNode ListNode;
typedef struct Data Data;

struct Data {
  int id;
  char* name;
  char* surname;
  int birthyear;
  char* address;
};

struct ListNode {
  ListNode* next;
  ListNode* prev;
  Data* data;
};

struct HashRow {
  int id;
  int listIndex; // -1 if removed
};

struct HashTable {
  int size;
  HashRow** rows;
};

struct LRUCache {
  int size;
  int count;
  HashTable* hashTable;
  ListNode* list; // head of the linked list.
};

/**
 * (3) Generic helper functions.
 */
char* mallocstr(int size) {
  char* str = (char*) malloc((size + 1) * sizeof(char));
  str[0] = '\0';
  return str;
}

char* copystr(char* str) {
  char* copy = mallocstr(strlen(str));
  if (copy) strcpy(copy, str);
  return copy;
}

/**
 * (4) Problem solution.
 */
LRUCache* createLRUCache(int cacheSize, int hashTableSize) {
  HashTable* hashTable = (HashTable*) malloc(sizeof(HashTable));
  hashTable->rows = (HashRow**) calloc(hashTableSize, sizeof(HashRow*));
  hashTable->size = hashTableSize;

  LRUCache* cache = (LRUCache*) malloc(sizeof(LRUCache));
  cache->size = cacheSize;
  cache->hashTable = hashTable;
  cache->list = NULL;
  cache->count = 0;

  return cache;
}

HashRow* createHashRow(int id) {
  HashRow* row = (HashRow*) malloc(sizeof(HashRow));
  row->id = id;
  row->listIndex = -1;
  return row;
}


ListNode* createListNode(Data* data) {
  ListNode* listNode = (ListNode*) malloc(sizeof(ListNode));
  listNode->data = data;
  listNode->next = NULL;
  listNode->prev = NULL;
  return listNode;
}

Data* createData(int id, char* name, char* surname, int birthyear, char* address) {
  Data* data = (Data*) malloc(sizeof(Data));
  data->id = id;
  data->name = copystr(name);
  data->surname = copystr(surname);
  data->birthyear = birthyear;
  data->address = copystr(address);
  return data;
}

// Calculate hash with polynomial hash function.
int calculateHash(int m, int i, int key) {
  int p = 31; // some fixed prime value
  unsigned long long int value = 0;

  // loop through all digits of key.
  // convert id into value to be hashed.
  int digit = 0;
  while (key != 0) {
    // get next digit.
    digit = key % 10;
    key /= 10;

    // calculate value to be hashed with horner's rule.
    value = p * value + digit;
  }

  // calculate hash of value with double hashing.
  int hash2 = 1 + (value % (m - 1));
  int hash1 = value % m;
  int hash = (hash1 + (i * hash2)) % m;

  return hash;
}

// Put given row into hash table.
void putIntoHashTable(HashTable* hashTable, HashRow* row) {
  bool found = false;
  int i = 0;
  int hash;

  // search for next available row.
  while (i < hashTable->size && !found) {
    hash = calculateHash(hashTable->size, i++, row->id);
    HashRow* currentRow = hashTable->rows[hash];
    found = (currentRow == NULL || currentRow->listIndex == -1); 
  }

  // if available row found
  if (i <= hashTable->size) {
    free(hashTable->rows[hash]);
    hashTable->rows[hash] = row;
  }
}

// Retrieve row with given id from hash table.
HashRow* getFromHashTable(HashTable* hashTable, int id) {
  int i = 0;

  // search for the row.
  while (i < hashTable->size) {
    int hash = calculateHash(hashTable->size, i++, id);
    HashRow* currentRow = hashTable->rows[hash];
    
    // this row has never been stored before.
    if (currentRow == NULL) return NULL;

    // found
    if (currentRow->id == id && currentRow->listIndex != -1) {
      return currentRow;
    }
  }

  // not found
  return NULL;
}

// Remove row with given id from hash table.
void removeFromHashTable(HashTable* hashTable, int id) {
  int i = 0;

  // search for the row.
  while (i < hashTable->size) {
    int hash = calculateHash(hashTable->size, i++, id);
    HashRow* currentRow = hashTable->rows[hash];
    
    // this row has never been stored before.
    if (currentRow == NULL) return;

    // found
    if (currentRow->id == id) {
      currentRow->listIndex = -1; // mark as removed
    }
  }
}

// Will increase indexes by one.
// If id specified, will run until the node with id found.
// If id not specified, will run for the all nodes.
void updateListIndexes(LRUCache* cache, int id) {
  ListNode* node = cache->list;
  int nodeId = node != NULL ? node->data->id : -1;
  bool cont = nodeId != -1 && nodeId != id;

  while (cont) {
    // Increase index by one.
    HashRow* row = getFromHashTable(cache->hashTable, nodeId);
    row->listIndex++;

    // Next node
    node = node->next;
    nodeId = node->data->id;

    // Should continue?
    cont = id != -1 ? // is id specified?
      (id != nodeId) : // yes: update until id
      (node != cache->list); // no: update all list
  }
}

ListNode* getNthNodeFromList(ListNode* list, int n) {
  while(n--) list = list->next;
  return list;
}

void makeNodeHead(LRUCache* cache, ListNode* node) {
  ListNode* head = cache->list;

  // If there is a different head.
  if (head != NULL && node != head) {
    ListNode* tail = head->prev;

    // Remove from place if node has prev & next.
    if (node->prev && node->next) {
      node->prev->next = node->next;
      node->next->prev = node->prev;
    }

    // Put node between head and tail.
    node->next = head;
    node->prev = tail;
    head->prev = node;
    tail->next = node;
  }

  // Make node the head.
  cache->list = node;
}

void putIntoCache(LRUCache* cache, Data* data) {
  // Check if there is place.
  // If not, remove last used data from cache.
  if (cache->count >= cache->size) {
    ListNode* lastNode = cache->list->prev;
    removeFromHashTable(cache->hashTable, lastNode->data->id);

    // remove from list.
    cache->list->prev = lastNode->prev;
    lastNode->prev->next = cache->list;
    free(lastNode);

    cache->count--;
  }

  // Update all indexes by one.
  updateListIndexes(cache, -1);

  // Create node and place at the top of the list.
  ListNode* node = createListNode(data);
  node->prev = node;
  node->next = node;
  makeNodeHead(cache, node);  

  // Place into hash table.
  HashRow* row = createHashRow(data->id);
  row->listIndex = 0;
  putIntoHashTable(cache->hashTable, row);

  cache->count++;
}

Data* getFromCache(LRUCache* cache, int id) {
  // Find through hash table.
  HashRow* row = getFromHashTable(cache->hashTable, id);
  if (row == NULL) return NULL;

  // If exist.
  ListNode* node = getNthNodeFromList(cache->list, row->listIndex);
  row->listIndex = 0;
  updateListIndexes(cache, node->data->id); // update to the left.
  makeNodeHead(cache, node);

  // return the data of the node.
  return node->data;
}

/**
 * (5) File processing and output.
 */
void printCacheState(LRUCache* cache) {
  HashTable* hashTable = cache->hashTable;
  ListNode* list = cache->list;
  int i = 0;

  // Create separator line.
  int separatorSize = 18 + 10 * hashTable->size;
  char separator[separatorSize];
  memset(separator, '-', separatorSize - 1);
  separator[separatorSize - 1] = '\0';

  printf("%s\n", separator);

  // Print nodes of linked list.
  printf("%-14s |  ", "List Nodes");
  if (list == NULL) printf("NULL");
  ListNode* node = list;
  do {
    if (node != NULL) {
      printf("%d", node->data->id);
      node = node->next;
      if (node != list) printf(" -> ");
      i++;
    }
  } while (node != list);

  if (i == cache->size) printf(" [FULL]");
  printf("\n%s\n", separator);

  // Print hash table header.
  printf("%-14s |  ", "HashTable Row");
  for (i = 0; i < hashTable->size; i++) {
    printf("%-10d", i);
  }
  printf("\n");

  // Print hash table body ids.
  printf("%-14s |  ", "  Id");
  for (i = 0; i < hashTable->size; i++) {
    HashRow* row = hashTable->rows[i];
    if (row != NULL) {
      printf("%-10d", row->id);
    } else {
      printf("%-10s", "-");
    }
  }
  printf("\n");

  // Print hash table body listIndexes.
  printf("%-14s |  ", "  ListIndex");
  for (i = 0; i < hashTable->size; i++) {
    HashRow* row = hashTable->rows[i];
    if (row != NULL) {
      printf("%-10d", row->listIndex);
    } else {
      printf("%-10s", "-");
    }
  }

  printf("\n%s\n", separator);
}

void processInputFile(LRUCache* cache, char* filename) {
  FILE* file = fopen(filename, "r");
  printf("Processing file '%s'...\n", filename);

  if (file == NULL) {
    printf("Error: Could not read file '%s'\n", filename);
    return;
  };

  printf("\nInitial Cache State:\n");
  printCacheState(cache);
  printf("\n");

  char* line = mallocstr(LINESIZE);
  int i = 0;
  int id;
  char* name;
  char* surname;
  int birthyear;
  char* address;

  // Loop through every line.
  while (fgets(line, LINESIZE, file)) {
    line[strcspn(line, "\r\n")] = 0;

    id = atoi(strtok(line, " "));

    printf("LINE #%d | ID: %d\n", ++i, id);
    Data* data = getFromCache(cache, id);
    
    if (data != NULL) {
      printf("-> Cache HIT! (#%d - %s %s - %d - %s)\n",
        data->id, data->name, data->surname, data->birthyear, data->address
      );
      printf("-> Entry retrieved. Current state of cache: \n");
      printCacheState(cache);
    } else {
      name = strtok(NULL, " ");
      surname = strtok(NULL, " ");
      birthyear = atoi(strtok(NULL, " "));
      address = strtok(NULL, " ");

      data = createData(id, name, surname, birthyear, address);
      putIntoCache(cache, data);
      
      printf("-> Cache MISS! (#%d - %s %s - %d - %s)\n",
        data->id, data->name, data->surname, data->birthyear, data->address
      );
      printf("-> Entry cached. Current state of cache: \n");

      printCacheState(cache);
    }

    printf("\n");
  }

  printf("Done.\n\n");

  fclose(file);
  free(line);
}

/**
 * (6) Main (Entry Point)
 */
int main() {
  // Print welcome message.
  printf("Welcome to YTULRU 2021\n");
  printf("Please provide following inputs.\n");

  // Ask for user input.
  int cacheSize;
  int hashtableSize;
  char* filename = mallocstr(LINESIZE);

  printf("Cache size: ");
  scanf("%d", &cacheSize);

  printf("HashTable size: ");
  scanf("%d", &hashtableSize);

  printf("Filename to process: ");
  scanf("%s", filename);
  
  LRUCache* cache = createLRUCache(cacheSize, hashtableSize);
  printf("\nLRU Cache created with Cache Size %d and HashTable Size %d.\n",
    cacheSize, hashtableSize
  );

  processInputFile(cache, filename);

  return 0;
}
