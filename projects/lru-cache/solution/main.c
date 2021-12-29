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
  // int nodeOrder;
  ListNode* listNode;
  bool isRemoved;
};

struct HashTable {
  int size;
  HashRow** rows;
};

struct LRUCache {
  int size;
  HashTable* hashTable;
  ListNode* list;
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

  LRUCache* lruCache = (LRUCache*) malloc(sizeof(LRUCache));
  lruCache->size = cacheSize;
  lruCache->hashTable = hashTable;
  lruCache->list = NULL;

  return lruCache;
}

HashRow* createHashRow(int id) {
  HashRow* row = (HashRow*) malloc(sizeof(HashRow));
  row->id = id;
  row->listNode = NULL;
  row->isRemoved = false;
  return row;
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
  unsigned long int value = 0;

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

// void cache(LRUCache* lruCache, Data* data) {

// }

// Data* getDataById(int id) {

// }

/**
 * () Main (Entry Point)
 */
int main() {
  LRUCache* lruCache = createLRUCache(10, 17);

  // int i;
  // for (i = 0; i < 17; i++) {
  //   printf("%d\n", calculateHash(17, i, 12345678));
  //   putIntoHashTable(lruCache->hashTable, createHashRow(12345678));
  // }

  // cache(lruCache, createData(1001, "Eric", "Cartman", 17, 11, 1917, "Turkey"));
  // cache(lruCache, createData(1002, "Sterling", "Archer", 18, 10, 1927, "USA"));
  // cache(lruCache, createData(1003, "Philip J.", "Fry", 19, 9, 1937, "England"));
  // cache(lruCache, createData(1004, "Peter", "Griffin", 20, 8, 1947, "Ireland"));

  return 0;
}
