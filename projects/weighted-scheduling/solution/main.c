#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/**
 * README
 */

/**
 * () Program configuration definitions.
 */
#define LINESIZE 256

/**
 * () Data types and structures.
 */
typedef struct Task Task; 
typedef struct Pool Pool;
typedef int (*IndexComparator) (void*, int, int);

struct Task {
  int start;
  int finish;
  int duration;
  int value;
};

struct Pool {
  Task** tasks;
  int count;
};

/**
 * () Generic helper functions.
 */
int* createIntArray(int len) {
  return (int*) calloc(len, sizeof(int));
}

/**
 * () Generic sorting implementation based on merge-sort.
 * Will work with any array filled with indexes.
 * Will call provided IndexComparator to decide the order.
 * In the end will return sorted index array.
 */
// Merging part of merge-sort.
void indexSortMerge(int* arr, int l, int m, int r, void* data, IndexComparator cmp) {
  int llen = m - l + 1;
  int rlen = r - m;

  // Create temporary left and right arrays.
  int left[llen];
  int right[rlen];
  memcpy(&left, (arr + l), llen * sizeof(int));
  memcpy(&right, (arr + m + 1), rlen * sizeof(int));

  // We'll merge left and right arrays back into original.
  int i, j, k;
  i = j = 0; // indexes in temp arrays.
  k = l; // index in original array.

  // Merge by comparing.
  while (i < llen && j < rlen) {
    arr[k++] = cmp(data, left[i], right[j]) <= 0 ?
      left[i++] : right[j++];
  }

  // Copy remaining elements.
  while (i < llen) arr[k++] = left[i++];
  while (j < rlen) arr[k++] = right[j++];
}

// Recursive part of merge-sort.
void indexSort(int* arr, int l, int r, void* data, IndexComparator cmp) {
  if (l < r) {
    int m = (l + r) / 2;
    indexSort(arr, l, m, data, cmp);
    indexSort(arr, m + 1, r, data, cmp);
    indexSortMerge(arr, l, m, r, data, cmp);
  }
}

// Comparator for custom data type.
int taskComparator(void* data, int i, int j) {
  Pool* pool = (Pool*) data; // Cast data type.
  Task* t1 = pool->tasks[i];
  Task* t2 = pool->tasks[j];
  return t1->finish - t2->finish;
}

// Main sort function for our data type Pool.
void sortPool(Pool* pool) {
  int* indexes = createIntArray(pool->count);

  // Create 0...n-1 index array.
  int i = 0;
  for (i = 0; i < pool->count; i++) indexes[i] = i;

  // Do index sort by our pool.
  indexSort(indexes, 0, pool->count - 1, pool, taskComparator);

  // Reflect sorted indexes into our pool.
  Task** tasks = (Task**) calloc(pool->count, sizeof(Task*));
  for (i = 0; i < pool->count; i++) {
    tasks[i] = pool->tasks[indexes[i]];
  }

  // Put sorted tasks in place, free previous one.
  free(pool->tasks);
  pool->tasks = tasks;
  
  free(indexes);
}

/**
 * () Problem Solution
 */
Task* createTask(int start, int duration, int value) {
  Task* task = (Task*) malloc(sizeof(Task));
  task->start = start;
  task->finish = start + duration;
  task->duration = duration;
  task->value = value;
  return task;
}

Pool* createPool(int count) {
  Pool* pool = (Pool*) malloc(sizeof(Pool)); 
  pool->tasks = (Task**) calloc(count, sizeof(Task*));
  pool->count = count;
  return pool;
}

void freePool(Pool* pool) {
  int i;
  for (i = 0; i < pool->count; i++) {
    free(pool->tasks[i]);
  }
  free(pool);
}

int findLatestNonOverlapingTaskIndex(Pool* pool, int taskIndex) {
  Task* t1 = pool->tasks[taskIndex];

  int i;
  for (i = taskIndex - 1; i >= 0; i--) {
    Task* t2 = pool->tasks[i];

    if (t2->finish <= t1->start) {
      return i;
    }
  }

  return -1;
}

int* calculateGains(Pool* pool) {
  // Initialize gain with first value.
  int* gains = createIntArray(pool->count);
  gains[0] = pool->tasks[0]->value;

  int i;
  for (i = 1; i < pool->count; i++) {
    int nextGain = pool->tasks[i]->value;
    int prevGain = gains[i - 1];

    // Check if there is previous non overlapping gain.
    int prev = findLatestNonOverlapingTaskIndex(pool, i);
    if (prev != -1) nextGain += gains[prev];

    // Choose bigger gain.
    gains[i] = nextGain > prevGain ? nextGain : prevGain;
  }

  return gains;
}

int* calculatePath(Pool* pool, int* gains) {
  int* path = createIntArray(pool->count);
  int j = 0;

  int i = pool->count - 1;
  while (i >= 0) {
    if (i == 0 || gains[i] != gains[i - 1]) {
      path[j++] = i + 1;
      i = findLatestNonOverlapingTaskIndex(pool, i);
    } else {
      i--;
    }
  }

  return path;
}

/**
 * () Extra: Reading tasks from file.
 * This is implemented for testing purposes.
 */

/**
 * () Getting tasks from user.
 */

/**
 * () Main (Entry Point)
 */
int main(int argc, char** argv) {
  int taskCount = 7;
  
  Pool* pool = createPool(taskCount);
  int i = 0;
  pool->tasks[i++] = createTask(9, 7, 1);
  pool->tasks[i++] = createTask(2, 5, 4);
  pool->tasks[i++] = createTask(10, 2, 2);
  pool->tasks[i++] = createTask(2, 9, 7);
  pool->tasks[i++] = createTask(5, 4, 4);
  pool->tasks[i++] = createTask(2, 13, 7);
  pool->tasks[i++] = createTask(1, 3, 2);
  
  sortPool(pool);

  int* gains = calculateGains(pool);
  int* path = calculatePath(pool, gains);

  for (int k = 0; k < taskCount; k++) {
    printf("%d ", gains[k]);
  }
  printf("\n");

  for (int k = 0; k < taskCount; k++) {
    printf("%d ", path[k]);
  }
  printf("\n");

  // Free and exit.
  freePool(pool);
  return 0;
}
