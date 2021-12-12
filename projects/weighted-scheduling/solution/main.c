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
  int id;
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
// Memory allocation and deallocations for our data types.
Task* createTask(int id, int start, int duration, int value) {
  Task* task = (Task*) malloc(sizeof(Task));
  task->id = id;
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

// Finding index of the nearest non-overlaping task
// in sorted pool from the left of the task with index j.
// with binary search approach.
int findLatestNonOverlapingTaskIndex(Pool* pool, int j) {
  Task** tasks = pool->tasks;
  int taskStart = tasks[j]->start;

  int l = 0;
  int r = j - 1;

  while (l <= r) {
    int m = (l + r) / 2;

    if (tasks[m]->finish <= taskStart) {
      if (tasks[m + 1]->finish <= taskStart) {
        l = m + 1;
      } else {
        return m;
      }
    } else {
      r = m - 1;
    }
  }

  return -1;
}

// Calculate step by step most possible gains.
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

    // Print flow.
    #ifndef TEST
    if (i == 1) {
      printf("Step 1\n");
      printf("-- Gain: %d\n", pool->tasks[0]->value);
      printf("-- Pick #1 [value=%d]\n", pool->tasks[0]->value);
    }
    printf("Step %d\n", i + 1);
    if (nextGain > prevGain) {
      printf("-- Gain: %d\n", nextGain);
      
      if (prev != i - 1) {
        printf("-- Drop #%d [value=%d] \n", i, pool->tasks[i-1]->value);
      }

      if (prev != -1) {
        printf("-- Pick #%d [value=%d] with #%d [gain=%d]\n", i + 1, pool->tasks[i]->value, prev + 1, gains[prev]);
      } else {
        printf("-- Pick #%d [value=%d]\n", i + 1, pool->tasks[i]->value);
      }
    } else {
      printf("-- Gain: %d\n", prevGain);
      printf("-- Prev chain is better.\n");
    }
    #endif
  }

  return gains;
}

// Calculate taken path of tasks by gains.
int* calculatePath(Pool* pool, int* gains) {
  int* path = createIntArray(pool->count);
  int i, j;

  i = j = pool->count - 1;
  while (i >= 0) {
    if (i == 0 || gains[i] != gains[i - 1]) {
      path[j--] = i + 1;
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
Pool* createPoolFromFile(char* filename) {
  FILE* file = fopen(filename, "r");

  if (file == NULL) {
    printf("Error: Could not read file '%s'\n", filename);
    return NULL;
  };

  // Create Pool with an initial size.
  Pool* pool = createPool(16);
  char* line = (char*) malloc((LINESIZE + 1) * sizeof(char));
  int index = 0;
  int start, duration, value;

  while (fgets(line, LINESIZE, file)) {
    // Reallocate memory for Pool if we need more.
    if (index == pool->count) {
      pool->count *= 2;
      pool->tasks = (Task**) realloc(pool->tasks, pool->count * sizeof(Task));
    }

    start = atoi(strtok(line, " "));
    duration = atoi(strtok(NULL, " "));
    value = atoi(strtok(NULL, " "));
    pool->tasks[index] = createTask(index + 1, start, duration, value);
    index++;
  }

  // Shrink memory pool of Pool if necessary.
  if (pool->count > index) {
    pool->count = index;
    pool->tasks = (Task**) realloc(pool->tasks, pool->count * sizeof(Task));
  }

  fclose(file);
  free(line);

  return pool;
}

/**
 * () Getting tasks from user.
 */
Pool* createPoolFromUserInput() {
  int count;
  printf("Please enter task (advert) count: ");
  scanf("%d", &count);

  // Create pool and fill the pool.
  Pool* pool = createPool(count);
  int start, duration, value;
  
  int i;
  for (i = 0; i < count; i++) {
    printf("  Enter details of task #%d:\n", i + 1);
    printf("    Start: ");
    scanf("%d", &start);
    printf("    Duration: ");
    scanf("%d", &duration);
    printf("    Value: ");
    scanf("%d", &value);

    pool->tasks[i] = createTask(i + 1, start, duration, value);
  }

  return pool;
}

/**
 * () Main (Entry Point)
 */
int main(int argc, char** argv) {
  Pool* pool = NULL;

  // Choose between reading from a file or user input.
  if (argc >= 2) {
    pool = createPoolFromFile(argv[1]);
  } else {
    pool = createPoolFromUserInput();
  }

  sortPool(pool);
  int* gains = calculateGains(pool);
  int* path = calculatePath(pool, gains);

  // Print results.
  int i;

  // Also print detailed results if not in test mode.
  #ifndef TEST
  printf("Done.\n\n");
  printf("Gains by steps: \n");
  for (i = 0; i < pool->count; i++) {
    printf("%d ", gains[i]);
  }
  printf("\n\n");
  printf("Total gain and path: \n");
  #endif

  // Print final result.
  printf("%d |", gains[pool->count - 1]);
  for (i = 0; i < pool->count; i++) {
    if (path[i] != 0) {
      printf(" %d", pool->tasks[path[i] - 1]->id);
    }
  }
  printf("\n");

  // Free and exit.
  freePool(pool);
  free(gains);
  free(path);
  return 0;
}
