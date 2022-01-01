#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/**
 * () Program configuration definitions.
 */
#define LINESIZE 256

/**
 * () Data types and structures.
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
  int bday;
  int bmonth;
  int byear;
  char* address;
};

struct ListNode {
  ListNode* next;
  ListNode* prev;
  Data* data;
};

struct HashRow {
  int id;
  ListNode* listNode;
  bool isRemoved;
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
 * () Generic helper functions.
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
 * () Problem solution.
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
  row->listNode = NULL;
  row->isRemoved = false;
  return row;
}


ListNode* createListNode(Data* data) {
  ListNode* listNode = (ListNode*) malloc(sizeof(ListNode));
  listNode->data = data;
  listNode->next = NULL;
  listNode->prev = NULL;
  return listNode;
}

Data* createData(int id, char* name, char* sname, int bday, int bmonth, int byear, char* address) {
  Data* data = (Data*) malloc(sizeof(Data));
  data->id = id;
  data->name = copystr(name);
  data->surname = copystr(sname);
  data->bday = bday;
  data->bmonth = bmonth;
  data->byear = byear;
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
    found = (currentRow == NULL || currentRow->isRemoved); 
  }

  // if available row found
  if (i <= hashTable->size) {
    printf("%d to %d\n", row->id, hash);
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
    if (currentRow->id == id && !currentRow->isRemoved) {
      printf("%d from %d\n", id, hash);
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
      currentRow->isRemoved = true;
    }
  }
}

void makeNodeHead(LRUCache* cache, ListNode* node) {
  ListNode* head = cache->list;

  // If there is a different head.
  if (head != NULL && node != head) {
    // Remove from place if node has prev & next.
    if (node->prev && node->next) {
      node->prev->next = node->next;
      node->next->prev = node->prev;
    }
    
    // Got tail.
    ListNode* tail = head->prev;

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
    if (cache->size == 1) {
      free(cache->list);
      cache->list = NULL;
    } else {
      cache->list->prev = lastNode->prev;
      lastNode->prev->next = cache->list;
      free(lastNode);
    }

    cache->count--;
  }
  
  // Create node and place at the top of the list.
  ListNode* node = createListNode(data);
  node->prev = node;
  node->next = node;

  makeNodeHead(cache, node);

  // Place into hash table.
  HashRow* row = createHashRow(data->id);
  row->listNode = node;
  putIntoHashTable(cache->hashTable, row);

  cache->count++;
}

Data* getFromCache(LRUCache* cache, int id) {
  // Find through hash table.
  HashRow* row = getFromHashTable(cache->hashTable, id);
  if (row == NULL) return NULL;

  // If exist.
  ListNode* node = row->listNode;
  makeNodeHead(cache, node);

  // return the data of the node.
  return node->data;
}

void printCache(LRUCache* cache) {
  HashTable* hashTable = cache->hashTable;
  ListNode* list = cache->list;
  
  printf("table ");
  int i;
  for (i = 0; i < hashTable->size; i++) {
    HashRow* row = hashTable->rows[i];

    if (row != NULL) {
      Data* data = row->listNode->data;
      printf("{%d: %d} ", i, row->isRemoved ? -1 : data->id);
    }
  }
  printf("\n");

  printf("list ");
  ListNode* node = list;
  i = 0;
  do {
    printf("{%d: #%d} ", i++, node->data->id);
    node = node->next;
  } while (node && node != list);

  printf("\n");
}

/**
 * () Main (Entry Point)
 */
int main() {
  LRUCache* cache = createLRUCache(7, 13);

  putIntoCache(cache, createData(1001, "Eric", "Cartman", 17, 11, 1917, "Turkey"));
  putIntoCache(cache, createData(1002, "Sterling", "Archer", 18, 10, 1927, "USA"));
  putIntoCache(cache, createData(1003, "Philip J.", "Fry", 19, 9, 1937, "England"));
  putIntoCache(cache, createData(1004, "Peter", "Griffin", 20, 8, 1947, "Ireland"));
  printCache(cache);

  printf("\nGet 2000\n");
  getFromCache(cache, 2000);
  printCache(cache);

  printf("\nGet 1002\n");
  getFromCache(cache, 1002);
  printCache(cache);

  printf("\nGet 1002\n");
  getFromCache(cache, 1002);
  printCache(cache);

  printf("\nPut 1005\n");
  putIntoCache(cache, createData(1005, "Leo", "Gunman", 20, 8, 1947, "Ireland"));
  printCache(cache);

  printf("\nPut 1006 and 1007\n");
  putIntoCache(cache, createData(1006, "Leo", "Gunman", 20, 8, 1947, "Ireland"));
  putIntoCache(cache, createData(1007, "Leo", "Gunman", 20, 8, 1947, "Ireland"));
  printCache(cache);

  printf("\nPut 1008\n");
  putIntoCache(cache, createData(1008, "Leo", "Gunman", 20, 8, 1947, "Ireland"));
  printCache(cache);

  printf("\nPut 1009-1015\n");
  putIntoCache(cache, createData(1009, "Leo", "Gunman", 20, 8, 1947, "Ireland"));
  printCache(cache);
  putIntoCache(cache, createData(1010, "Leo", "Gunman", 20, 8, 1947, "Ireland"));
  printCache(cache);
  putIntoCache(cache, createData(1011, "BinONBIR", "Gunman", 20, 8, 1947, "Ireland"));
  printCache(cache);
  putIntoCache(cache, createData(1012, "Leo", "Gunman", 20, 8, 1947, "Ireland"));
  printCache(cache);
  putIntoCache(cache, createData(1013, "Leo", "Gunman", 20, 8, 1947, "Ireland"));
  printCache(cache);
  putIntoCache(cache, createData(1014, "Leo", "Gunman", 20, 8, 1947, "Ireland"));
  printCache(cache);
  putIntoCache(cache, createData(1015, "Leo", "Gunman", 20, 8, 1947, "Ireland"));
  printCache(cache);

  Data* data = getFromCache(cache, 1011);
  printf("Got %s\n", data->name);
  printCache(cache);

  return 0;
}
