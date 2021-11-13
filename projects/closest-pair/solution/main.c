#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

/**
 * () Program configuration definitions.
 */
#define LINESIZE 256
#define DEFAULT_INPUT "sample.txt"

/**
 * () Data types and structures.
 */
typedef struct Point Point;
typedef struct Space Space;
typedef struct Pair Pair;

// A point in Space. Space is 2D in this program.
struct Point {
  int x;
  int y;
};

// Space is collection of Points.
struct Space {
  Point** points;
  int count;
};

// Pair is two paired Points.
struct Pair {
  Point* a;
  Point* b;
  float distance;
};

/**
 * () Common generic utilities.
 */
char* mallocstr(int size) {
  return (char*) malloc((size + 1) * sizeof(char));
}

/**
 * () QuickSort Algorithm for Space. (aka "QuickSortSpace", qss)
 */
typedef int (*PointCmp) (Point*, Point*);

void qssSwap(Point** a, Point** b) { 
  Point* point = *a; 
  *a = *b; 
  *b = point; 
}

int qssPartition(Space* space, int left, int right, PointCmp cmp) {
  Point** points = space->points;
  Point* pivot = points[right];
  int i = (left - 1);
  int j;

  for (j = left; j <= right - 1; j++) {
    if (cmp(points[j], pivot) < 0) { 
      qssSwap(&points[++i], &points[j]); 
    } 
  }

  qssSwap(&points[i + 1], &points[right]); 
  return (i + 1); 
}

void qss(Space* space, int left, int right, PointCmp cmp) { 
  if (left < right) { 
    int pi = qssPartition(space, left, right, cmp); 
    qss(space, left, pi - 1, cmp); 
    qss(space, pi + 1, right, cmp); 
  } 
}

void sortSpace(Space* space, PointCmp cmp) {
  qss(space, 0, space->count - 1, cmp);
}

int sortSpaceCompareX(Point* a, Point* b) {
  return (a->x != b->x) ? (a->x - b->x) : (a->y - b->y);
}

int sortSpaceCompareY(Point* a, Point* b) {
  return (a->y != b->y) ? (a->y - b->y) : (a->x - b->x);
}

/**
 * () Space and Point operations.
 */
Point* createPoint(int x, int y) {
  Point* point = (Point*) malloc(sizeof(Point));
  point->x = x;
  point->y = y;
  return point;
}

void freePoint(Point* point) {
  free(point);
}

Space* createSpace(int count) {
  Space* space = (Space*) malloc(sizeof(Space));
  space->points = (Point**) calloc(count, sizeof(Point));
  space->count = count;
  return space;
}

Space* copySpace(Space* space) {
  Space *copy = createSpace(space->count);
  int i;
  for(i = 0; i < space->count; i++) {
    copy->points[i] = createPoint(space->points[i]->x, space->points[i]->y);
  }
  return copy;
}

// Read collection of points (aka Space) from a txt file.
// Each line of file should has space separated x y information.
Space* readSpaceFromFile(char* filename) {
  FILE* file = fopen(filename, "r");

  if (file == NULL) {
    printf("Error: Could not read file '%s'", filename);
    return NULL;
  };  

  // Create Space with an initial size.
  Space* space = createSpace(16);
  char* line = mallocstr(LINESIZE);
  int index = 0;

  while (fgets(line, LINESIZE, file)) {
    // Reallocate memory for Space if we need more.
    if (index == space->count) {
      space->count *= 2;
      space->points = (Point**) realloc(space->points, space->count * sizeof(Point));
    }

    Point* point = createPoint(0, 0);
    point->x = atoi(strtok(line, " "));
    point->y = atoi(strtok(NULL, " "));
    space->points[index++] = point;
  }

  // Shrink memory space of Space if necessary.
  if (space->count > index) {
    space->count = index;
    space->points = (Point**) realloc(space->points, space->count * sizeof(Point));
  }

  fclose(file);
  free(line);
  return space;
}

void freeSpace(Space* space) {
  int i;
  for (i = 0; i < space->count; i++) {
    freePoint(space->points[i]);
  }
  free(space->points);
  free(space);
}

/**
 * () Closest Pair Calculation
 */
int dist(Point* a, Point* b) {
  return sqrt(pow(a->x-b->x, 2) + pow(a->y-b->y, 2));
}

float min(float a, float b) {
  return (a < b) ? a : b;
}

Pair* createPair(Point* a, Point* b, float distance) {
  Pair* pair = (Pair*) malloc(sizeof(Pair));
  pair->a = createPoint(a->x, a->y);
  pair->b = createPoint(b->x, b->y);
  pair->distance = distance;
  return pair;
}

void freePair(Pair* pair) {
  freePoint(pair->a);
  freePoint(pair->b);
  free(pair);
}

Pair* findClosestPair(Space* space) {
  // return pair directly if n == 2

  // do brute-force if n <= 3

  // find middle point by x
  

  // printf("mid: %d %d\n", mid->x, mid->y);

  // remove
  Point* a = space->points[0];
  Point* b = space->points[1];
  Pair* pair = createPair(a, b, dist(a,b));
  return pair;
}

/**
 * () Main
 */
int main() {
  Space* space = readSpaceFromFile("sample.txt");
  Pair* pair = findClosestPair(space);

  Space* spaceX = copySpace(space);
  Space* spaceY = copySpace(space);

  sortSpace(spaceX, sortSpaceCompareX);
  sortSpace(spaceY, sortSpaceCompareY);

  space = spaceX;
  printf("by x\n");
  for(int i=0; i < space->count; i++) {
    printf("%d %d\n", space->points[i]->x, space->points[i]->y);
  }

  space = spaceY;
  printf("\n\nby y\n");
  for(int i=0; i < space->count; i++) {
    printf("%d %d\n", space->points[i]->x, space->points[i]->y);
  }

  freeSpace(space);
  freePair(pair);
  return 0;
}
