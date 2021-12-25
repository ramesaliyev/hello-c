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
#define INDENT " "

/**
 * (2) Data types and structures.
 */
typedef struct Cost Cost;
typedef struct City City;
typedef struct Graph Graph;
typedef struct Path Path;
typedef struct Paths Paths;
typedef int (*IndexComparator) (void*, int, int);

enum SortBy {SortByNull, SortByPrice, SortByDuration}; 
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

FILE* openFile(char* filename, char* as) {
  FILE* file = fopen(filename, as);
  if (file == NULL) {
    printf("Error: Could not open file; name=%s, mode=%s\n", filename, as);
    return NULL;
  };
  return file;
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

int pathPriceComparator(void* data, int i, int j) {
  Paths* paths = (Paths*) data; // Cast data type.
  Path* p1 = paths->paths[i];
  Path* p2 = paths->paths[j];
  return p1->price - p2->price;
}

int pathDurationComparator(void* data, int i, int j) {
  Paths* paths = (Paths*) data; // Cast data type.
  Path* p1 = paths->paths[i];
  Path* p2 = paths->paths[j];
  return p1->duration - p2->duration;
}

void sortPaths(Paths* paths, SortBy sortBy) {
  int* indexes = createIntArray(paths->count);

  // Create 0...n-1 index array.
  int i = 0;
  for (i = 0; i < paths->count; i++) indexes[i] = i;

  // Determine comparator.
  IndexComparator pathComparator = sortBy == SortByPrice ? 
    pathPriceComparator : pathDurationComparator;

  // Do index sort by our paths.
  indexSort(indexes, 0, paths->count - 1, paths, pathComparator);

  // Reflect sorted indexes into our pool.
  Path** npaths = (Path**) calloc(paths->count, sizeof(Path*));
  for (i = 0; i < paths->count; i++) {
    npaths[i] = paths->paths[indexes[i]];
  }

  // Put sorted tasks in place, free previous one.
  free(paths->paths);
  paths->paths = npaths;
  
  free(indexes);
}

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

void dfsPath(Graph* graph, Paths* paths, int* path, int i) {
  int node = i == 0 ? paths->from : path[i - 1];

  int j;
  for (j = 0; j < graph->cityCount; j++) {
    if (graph->costs[node][j] != NULL) {
      if (j == paths->to) {
        addPath(graph, paths, path, i);
      } else if (i < paths->stops && notInPath(paths, path, i, j)) {
        int* subpath = copyIntArray(path, paths->stops);
        subpath[i] = j;
        dfsPath(graph, paths, subpath, i + 1);
      }
    }    
  }

  free(path);
}

Paths* findPaths(Graph* graph, int from, int to, int stops) {
  Paths* paths = createPaths(from, to, stops);
  dfsPath(graph, paths, createIntArray(stops), 0);
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
 * (7) Handle User Interaction.
 */
void saveSearchResults(Graph* graph, Paths* paths, SortBy sortBy) {
  char filename[LINESIZE];
  printf("Filename: ");
  scanf("%s", filename);

  FILE* file = openFile(filename, "wb");

  // Create separator line.
  char separator[101];
  memset(separator, '-', 100);
  separator[100] = '\0';

  fprintf(file, "%s\n", separator);
  fprintf(file, "Flights from %s to %s with at most %d stops. Sorted by %s.\n",
    graph->cities[paths->from]->name,
    graph->cities[paths->to]->name,
    paths->stops, sortBy == SortByDuration ? "duration" : "price"
  );
  fprintf(file, "%s\n", separator);
  fprintf(file, "%-15s%-15s%-40s%-10s%-10s%-10s\n",
    "Source", "Destination", "Stops", "Hour", "Minute", "Price");
  fprintf(file, "%s\n", separator);
  
  int i, j;
  for (i = 0; i < paths->count; i++) {
    fprintf(file, "%-15s", graph->cities[paths->from]->name);
    fprintf(file, "%-15s", graph->cities[paths->to]->name);

    Path* path = paths->paths[i];

    if (path->stops == 0) {
      fprintf(file, "%-40s", "None");
    } else {
      char* stopNames = mallocstr(LINESIZE);
      
      for (j = 0; j < path->stops; j++) {
        int cityId = path->path[j];

        if (j != 0) strcat(stopNames, ", ");
        strcat(stopNames, graph->cities[cityId]->name);
      }

      fprintf(file, "%-40s", stopNames);
      free(stopNames);
    }

    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%dh", path->duration / 60);
    fprintf(file, "%-10s", buffer);
    snprintf(buffer, sizeof(buffer), "%dm", path->duration % 60);
    fprintf(file, "%-10s", buffer);
    snprintf(buffer, sizeof(buffer), "%d$", path->price);
    fprintf(file, "%-10s", buffer);

    fprintf(file, "\n");
  }

  printf("Search results saved into file %s\n\n", filename);
  fclose(file);
}

int interactiveUserSearch(Graph* graph) {
  // Search and sort criteria.
  char from[LINESIZE];
  char to[LINESIZE];
  char sort[LINESIZE];
  int stops = 0;
  SortBy sortBy = SortByNull;
  int fromId = -1;
  int toId = -1;

  // Get user input.
  printf("Please enter search criteria:\n");
  
  while (fromId == -1) {
    printf(INDENT"From: ");
    scanf("%s", from);

    fromId = getCityIdByName(graph, from);
    if (fromId == -1) {
      printf(INDENT"Unknown city specified.\n\n");
    }
  }

  while (toId == -1) {
    printf(INDENT"To: ");
    scanf("%s", to);

    toId = getCityIdByName(graph, to);
    if (toId == -1) {
      printf(INDENT"Unknown city specified.\n\n");
    } else if (toId == fromId) {
      printf(INDENT"Source and destination cannot be the same city.\n\n");
      toId = -1;
    }
  }

  printf(INDENT"Stops (0 for direct flights): ");
  scanf("%d", &stops);
  
  while (sortBy == SortByNull) {
    printf(INDENT"Sort criterion? Enter p for price, d for duration: ");
    scanf("%s", sort);

    if (strcmp(sort, "p") == 0) {
      sortBy = SortByPrice;
    } else if (strcmp(sort, "d") == 0) {
      sortBy = SortByDuration;
    } else {
      printf(INDENT"Unknown sort criterion.\n\n");
    }
  }

  // Find all possible flight paths.
  Paths* paths = findPaths(graph, fromId, toId, stops);
  sortPaths(paths, sortBy);

  // Create separator line.
  char separator[101];
  memset(separator, '-', 100);
  separator[100] = '\0';

  if (paths->count == 0) {
    Paths* pathsWithStops = findPaths(graph, fromId, toId, graph->cityCount - 2);

    printf("\n");
    if (stops == 0) {
      printf("No direct flight found between given cities.\n");
    } else {
      printf("No flights found for given criteria.\n");
    }

    if (pathsWithStops->count == 0) {
      printf("There are no possible flight scenarios between given cities.\n");
      printf("Not with even %d stops.\n", graph->cityCount - 2);
    } else {
      printf("But there are flights between given cities with more stops.\n");
    }
    
    printf("\n");
    return 0;
  }

  printf("%s\n", separator);
  printf("Flights from %s to %s with at most %d stops. Sorted by %s.\n",
    from, to, stops, sortBy == SortByDuration ? "duration" : "price");
  printf("%s\n", separator);
  printf("%-15s%-15s%-40s%-10s%-10s%-10s\n",
    "Source", "Destination", "Stops", "Hour", "Minute", "Price");
  printf("%s\n", separator);

  int i, j;
  for (i = 0; i < paths->count; i++) {
    printf("%-15s", graph->cities[paths->from]->name);
    printf("%-15s", graph->cities[paths->to]->name);

    Path* path = paths->paths[i];

    if (path->stops == 0) {
      printf("%-40s", "None");
    } else {
      char* stopNames = mallocstr(LINESIZE);
      
      for (j = 0; j < path->stops; j++) {
        int cityId = path->path[j];

        if (j != 0) strcat(stopNames, ", ");
        strcat(stopNames, graph->cities[cityId]->name);
      }

      printf("%-40s", stopNames);
      free(stopNames);
    }

    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%dh", path->duration / 60);
    printf("%-10s", buffer);
    snprintf(buffer, sizeof(buffer), "%dm", path->duration % 60);
    printf("%-10s", buffer);
    snprintf(buffer, sizeof(buffer), "%d$", path->price);
    printf("%-10s", buffer);

    printf("\n");
  }
  printf("%s\n", separator);
  printf("Commands:\n");
  printf(INDENT"save - save results to file \n");
  printf(INDENT"search - do another search \n");
  printf(INDENT"exit - exit from program \n");
  printf("\n");

  char cmd[LINESIZE];
  
  while (true) {
    scanf("%s", cmd);

    if (strcmp(cmd, "save") == 0) {
      saveSearchResults(graph, paths, sortBy);
      free(paths);
      return 0;
    } else if (strcmp(cmd, "search") == 0) {
      free(paths);
      return 1;
    } else if (strcmp(cmd, "exit") == 0) {
      exit(0);
    } else {
      printf("Unknown command.\n\n");
    }
  }

  free(paths);
  return 0;
}

/**
 * (8) Main (Entry Point)
 */
int main(int argc, char** argv) {
  char* filename = DEFAULT_INPUT;
  if (argc >= 2) filename = argv[1];

  // Read file and create graph from it.
  Graph* graph = createGraphFromFile(filename);
  if (graph == NULL) return 1;

  // Print welcome message.
  printf("Welcome to YTUSkyScanner 2021\n");

  // Ask for user input.
  while (true) {
    printf("Commands:\n");
    printf(INDENT"search - search for flights \n");
    printf(INDENT"exit - exit from program \n");
    printf("\n");

    char cmd[LINESIZE];
    scanf("%s", cmd);

    if (strcmp(cmd, "exit") == 0) {
      return 0;
    } else if (strcmp(cmd, "search") == 0) {
      while (interactiveUserSearch(graph));
    } else {
      printf("Unknown command.\n\n");
    }
  }
  
  return 0;
}
