#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/**
 * README
 */
typedef struct Task Task;
typedef struct Pool Pool;

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

int* createIntArray(int len) {
  return (int*) calloc(len, sizeof(int));
}

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

int main(int argc, char** argv) {
  int taskCount = 7;
  
  Pool* pool = createPool(taskCount);
  pool->tasks[0] = createTask(1, 3, 2);
  pool->tasks[1] = createTask(2, 5, 4);
  pool->tasks[2] = createTask(5, 4, 4);
  pool->tasks[3] = createTask(2, 9, 7);
  pool->tasks[4] = createTask(10, 2, 2);
  pool->tasks[5] = createTask(2, 13, 7);
  pool->tasks[6] = createTask(9, 7, 1);
  
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

// Read from user.
// Read from file.
// Order task-pool.
