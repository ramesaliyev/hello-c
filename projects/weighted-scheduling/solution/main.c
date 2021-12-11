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
void mergeIndexes(int* arr, int l, int m, int r, void* data, IndexComparator cmp) {
  int i, j, k;
  int n1 = m - l + 1;
  int n2 = r - m;

  // Create left and right arrays.
  int L[n1], R[n2];
  /* Copy data to temp arrays L[] and R[] */
  for (i = 0; i < n1; i++)
    L[i] = arr[l + i];
  for (j = 0; j < n2; j++)
    R[j] = arr[m + 1 + j];

  /* Merge the temp arrays back into arr[l..r]*/
  i = 0; // Initial index of first subarray
  j = 0; // Initial index of second subarray
  k = l; // Initial index of merged subarray
  while (i < n1 && j < n2) {
      if (cmp(data, L[i], R[j]) <= 0) {
          arr[k] = L[i];
          i++;
      }
      else {
          arr[k] = R[j];
          j++;
      }
      k++;
  }

  /* Copy the remaining elements of L[], if there
  are any */
  while (i < n1) {
      arr[k] = L[i];
      i++;
      k++;
  }

  /* Copy the remaining elements of R[], if there
  are any */
  while (j < n2) {
      arr[k] = R[j];
      j++;
      k++;
  }
}

void indexSort(int* arr, int l, int r, void* data, IndexComparator cmp) {
  if (l < r) {
    int m = l + (r - l) / 2;
    indexSort(arr, l, m, data, cmp);
    indexSort(arr, m + 1, r, data, cmp);
    mergeIndexes(arr, l, m, r, data, cmp);
  }
}

int taskComparator(void* data, int i, int j) {
  Pool* pool = (Pool*) data; // Cast data type.
  Task* t1 = pool->tasks[i];
  Task* t2 = pool->tasks[j];
  return t1->finish - t2->finish;
}

void sortPool(Pool* pool) {
  // Create 0...n-1 index array.
  int n = pool->count;
  int i = 0;
  int* indexes = createIntArray(n);
  for (i = 0; i < n; i++) indexes[i] = i;

  // Do index sort by our pool.
  indexSort(indexes, 0, n - 1, pool, taskComparator);

  for (int k = 0; k < n; k++) {
    printf("%d ", indexes[k]);
  }
  printf("\n");
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

int findLatestNonOverlapingTaskIndex(Pool* pool, int taskIndex) {
  Task* t1 = pool->tasks[taskIndex];

  int j;
  for (j = taskIndex - 1; j >= 0; j--) {
    Task* t2 = pool->tasks[j];

    if (t2->finish <= t1->start) {
      return j;
    }
  }

  return -1;
}

int* calculateGains(Pool* pool) {
  // Initialize gain with first value.
  int* gains = createIntArray(pool->count);
  gains[0] = pool->tasks[0]->value;

  int j;
  for (j = 1; j < pool->count; j++) {
    int nextGain = pool->tasks[j]->value;
    int prevGain = gains[j - 1];

    // Check if there is previous non overlapping gain.
    int prev = findLatestNonOverlapingTaskIndex(pool, j);
    if (prev != -1) nextGain += gains[prev];

    // Choose bigger gain.
    gains[j] = nextGain > prevGain ? nextGain : prevGain;
  }

  return gains;
}

int* calculatePath(Pool* pool, int* gains) {
  int* path = createIntArray(pool->count);
  int i = 0;

  int j = pool->count - 1;
  while (j >= 0) {
    if (j == 0 || gains[j] != gains[j - 1]) {
      path[i++] = j + 1;
      j = findLatestNonOverlapingTaskIndex(pool, j);
    } else {
      j--;
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

  return 0;
}
