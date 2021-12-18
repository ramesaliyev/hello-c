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
 * (4) Generic sorting implementation based on merge-sort.
 * (5) Problem Solution
 * (6) Reading flights from file.
 * (7) Getting user input.
 * (8) Main (Entry Point)
 * 
 */

/**
 * (1) Program configuration definitions.
 */
#define LINESIZE 256
#define DEFAULT_INPUT "sample.txt"

/**
 * (2) Data types and structures.
 */
typedef struct Cost Cost;
typedef struct City City;
typedef struct Graph Graph;
typedef struct Path Path;
typedef struct Paths Paths;
typedef int (*IndexComparator) (void*, int, int);

enum SortBy {SortByPrice, SortByDuration}; 
typedef enum SortBy SortBy;

struct Cost {
  int price;
  int duration; // in minutes
};

struct City {
  int id; // starts from 0
  char* name;
};

struct Graph {
  int cityCount;
  City** cities; // vertices
  Cost*** costs; // edges (adjacency matrix)
};

struct Path {
  int stops; // actual stops count of path
  int price; // total price of path
  int duration; // total duration of path (durations + (stops*60))
  int* path; // array of city ids
};

struct Paths {
  int from;
  int to;
  int count; // total number of paths
  int stops; // max stops in paths
  Path** paths;
};

/**
 * (3) Generic helper functions.
 */
char* mallocstr(int size) {
  return (char*) malloc((size + 1) * sizeof(char));
}

char* copystr(char* str) {
  char* copy = mallocstr(strlen(str));
  if (copy) strcpy(copy, str);
  return copy;
}

int* createIntArray(int len) {
  return (int*) calloc(len, sizeof(int));
}

int* copyIntArray(int* arr, int len) {
  int* copy = createIntArray(len);
  memcpy(copy, arr, len * sizeof(int));
  return copy;
}

bool inArray(int* arr, int len, int val) {
  int i;
  for (i = 0; i < len; i++) {
    if (arr[i] == val) return true;
  }
  return false;
}

/**
 * (4) Generic sorting implementation based on merge-sort.
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
// int taskComparator(void* data, int i, int j) {
//   Pool* pool = (Pool*) data; // Cast data type.
//   Task* t1 = pool->tasks[i];
//   Task* t2 = pool->tasks[j];
//   return t1->finish - t2->finish;
// }

// Main sort function for our data type Pool.
// void sortPool(Pool* pool) {
//   int* indexes = createIntArray(pool->count);

//   // Create 0...n-1 index array.
//   int i = 0;
//   for (i = 0; i < pool->count; i++) indexes[i] = i;

//   // Do index sort by our pool.
//   indexSort(indexes, 0, pool->count - 1, pool, taskComparator);

//   // Reflect sorted indexes into our pool.
//   Task** tasks = (Task**) calloc(pool->count, sizeof(Task*));
//   for (i = 0; i < pool->count; i++) {
//     tasks[i] = pool->tasks[indexes[i]];
//   }

//   // Put sorted tasks in place, free previous one.
//   free(pool->tasks);
//   pool->tasks = tasks;
  
//   free(indexes);
// }

/**
 * (5) Problem Solution
 */
Graph* createGraph(int cityCount) {
  Graph* graph = (Graph*) malloc(sizeof(Graph));
  graph->cities = (City**) calloc(cityCount, sizeof(City*));
  graph->cityCount = cityCount;
  return graph;
}

void createGraphCostsMatrix(Graph* graph) {
  int i; 
  graph->costs = (Cost***) calloc(graph->cityCount, sizeof(Cost**));
  for (i = 0; i < graph->cityCount; i++) {
    graph->costs[i] = (Cost**) calloc(graph->cityCount, sizeof(Cost*));
  }
}

City* createCity(int id, char* name) {
  City* city = (City*) malloc(sizeof(City));
  city->id = id;
  city->name = copystr(name);
  return city;
}

Cost* createCost(int price, int hours, int minutes) {
  Cost* cost = (Cost*) malloc(sizeof(Cost));
  cost->price = price;
  cost->duration = (hours * 60) + minutes;
  return cost;
}

Paths* createPaths(int from, int to, int stops) {
  Paths* paths = (Paths*) malloc(sizeof(Paths));
  paths->paths = NULL;
  paths->stops = stops;
  paths->from = from;
  paths->to = to;
  paths->count = 0;
  return paths;
}

Path* createPath(Graph* graph, int* patharr, int from, int to, int stops) {
  Path* path = (Path*) malloc(sizeof(Path));
  path->stops = stops;
  path->duration = stops * 60;
  path->price = 0;
  path->path = copyIntArray(patharr, stops);

  int i, a, b;
  for (i = 0; i <= stops; i++) {
    a = i == 0 ? from : patharr[i - 1]; // left city
    b = i == stops ? to : patharr[i]; // right city

    Cost* cost = graph->costs[a][b];
    path->duration += cost->duration;
    path->price += cost->price;
  }

  return path;
} 

void addPath(Graph* graph, Paths* paths, int* path, int stops) {
  paths->paths = (Path**) realloc(paths->paths, (paths->count + 1) * sizeof(Path*));
  paths->paths[paths->count] = createPath(graph, path, paths->from, paths->to, stops);
  paths->count++;
}

bool notInPath(Paths* paths, int* path, int len, int val) {
  return (val != paths->from) && !inArray(path, len, val);
}

int getCityIdByName(Graph* graph, char* name) {
  int i;
  for (i = 0; i < graph->cityCount; i++) {
    if (graph->cities[i] == NULL) return -1;
    if (strcmp(graph->cities[i]->name, name) == 0) return i;
  }
  return -1;
}

void bfsPath(Graph* graph, Paths* paths, int* path, int i) {
  int id = i == 0 ? paths->from : path[i - 1];

  int j;
  for (j = 0; j < graph->cityCount; j++) {
    Cost* cost = graph->costs[id][j];

    if (cost != NULL) {
      if (j == paths->to) {
        addPath(graph, paths, path, i);
      } else if (i < paths->stops && notInPath(paths, path, i, j)) {
        int* subpath = copyIntArray(path, paths->stops);
        subpath[i] = j;
        bfsPath(graph, paths, subpath, i + 1);
      }
    }    
  }

  free(path);
}

Paths* findPaths(Graph* graph, int from, int to, int stops) {
  Paths* paths = createPaths(from, to, stops);
  bfsPath(graph, paths, createIntArray(stops), 0);
  return paths;
}

/**
 * (6) Reading flights from file.
 */
Graph* createGraphFromFile(char* filename) {
  FILE* file = fopen(filename, "r");

  if (file == NULL) {
    printf("Error: Could not read file '%s'\n", filename);
    return NULL;
  };

  // Allocate graph with initial city count.
  Graph* graph = createGraph(4);

  // Variables to collect cities.
  char* line = (char*) malloc((LINESIZE + 1) * sizeof(char));
  int nextCityId = 0;
  char* from;
  char* to;

  // Collect and count how many cities are in the file.
  while (fgets(line, LINESIZE, file)) {
    if ((nextCityId + 1) >= graph->cityCount) {
      graph->cityCount *= 2;
      graph->cities = (City**) realloc(graph->cities, graph->cityCount * sizeof(City*));
    }
    
    from = strtok(line, " ");
    to = strtok(NULL, " ");

    if (getCityIdByName(graph, from) == -1) {
      graph->cities[nextCityId] = createCity(nextCityId, from);
      nextCityId++;
    }

    if (getCityIdByName(graph, to) == -1) {
      graph->cities[nextCityId] = createCity(nextCityId, to);
      nextCityId++;
    }
  }

  // Shrink allocated memory of cities if necessary.
  if (graph->cityCount > nextCityId) {
    graph->cityCount = nextCityId;
    graph->cities = (City**) realloc(graph->cities, graph->cityCount * sizeof(City*));
  }

  // Rewind to the beginning of file.
  rewind(file);

  // Create costs matrix (adjacency matrix).
  createGraphCostsMatrix(graph);

  int hours, minutes, price;
  int fromId, toId;

  // Collect and place the costs in the adjacency matrix.
  while (fgets(line, LINESIZE, file)) {
    from = strtok(line, " ");
    to = strtok(NULL, " ");
    hours = atoi(strtok(NULL, " "));
    minutes = atoi(strtok(NULL, " "));
    price = atoi(strtok(NULL, " "));

    fromId = getCityIdByName(graph, from);
    toId = getCityIdByName(graph, to);

    Cost* cost = createCost(price, hours, minutes);
    graph->costs[fromId][toId] = graph->costs[toId][fromId] = cost;
  }

  fclose(file);
  free(line);
  return graph;
}

/**
 * (7) Getting user input.
 */

/**
 * (8) Main (Entry Point)
 */
int main(int argc, char** argv) {
  char* filename = DEFAULT_INPUT;
  if (argc >= 2) filename = argv[1];

  // Read file and create graph from it.
  Graph* graph = createGraphFromFile(filename);
  if (graph == NULL) return 1;

  // Ask for user input.
  char* from = "Londra";
  char* to = "Paris";

  int fromId = getCityIdByName(graph, from);
  int toId = getCityIdByName(graph, to);
  int stops = 30;

  // Find all possible flight paths.
  Paths* paths = findPaths(graph, fromId, toId, stops);

  printf("%s --> %s\n\n", from, to);
  int i, j;
  for (i = 0; i<paths->count; i++) {
    Path* path = paths->paths[i];

    if (path->stops == 0) {
      printf("DIRECT ");
    } else {
      for (j = 0; j < path->stops; j++) {
        int id = path->path[j];
        if (id != -1) {
          printf("%s ", graph->cities[id]->name);
        }
      }
    }
    
    printf("--- %d$ %dm", path->price, path->duration);
    printf("\n");
  }

  free(paths);
  return 0;
}
