#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <float.h>
#include <math.h>

/**
 * README
 */

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

typedef int (*PointCmp) (Point*, Point*); // Point comparator function type.

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
 * () QuickSort Algorithm for Space. (aka "QuickSortSpace", qss)
 */
void qssSwap(Point** a, Point** b) { 
  Point* tmp = *a; 
  *a = *b; 
  *b = tmp; 
}

int qssPartition(Space* space, int left, int right, PointCmp cmp) {
  Point** points = space->points;
  Point* pivot = points[left];
  
  int i;
  int j = right;
  
  for (i = left + 1; i <= j; i++) {
    if (cmp(points[i], pivot) > 0) { 
      qssSwap(&points[i--], &points[j--]);
    }
  }

  qssSwap(&points[left], &points[j]); 
  return j; 
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
Space* createSpace(int count) {
  Space* space = (Space*) malloc(sizeof(Space));
  space->points = (Point**) calloc(count, sizeof(Point));
  space->count = count;
  return space;
}

Space* sliceSpace(Space* space, int start, int end) {
  int count = end - start;
  Space *copy = createSpace(count);
  memcpy(copy->points, (space->points + start), count * (sizeof(Point)/sizeof(char)));
  return copy;
}

Space* copySpace(Space* space) {
  return sliceSpace(space, 0, space->count);
}

// Read collection of points (aka Space) from a txt file.
// Each line of file should has space separated x y information.
Space* createSpaceFromFile(char* filename) {
  FILE* file = fopen(filename, "r");

  if (file == NULL) {
    printf("Error: Could not read file '%s'", filename);
    return NULL;
  };  

  // Create Space with an initial size.
  Space* space = createSpace(16);
  char* line = (char*) malloc((LINESIZE + 1) * sizeof(char));
  int index = 0;

  while (fgets(line, LINESIZE, file)) {
    // Reallocate memory for Space if we need more.
    if (index == space->count) {
      space->count *= 2;
      space->points = (Point**) realloc(space->points, space->count * sizeof(Point));
    }

    Point* point = (Point*) malloc(sizeof(Point));
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
  free(space->points);
  free(space);
}

void freePointsOfSpace(Space* space) {
  int i;
  for (i = 0; i < space->count; i++) {
    free(space->points[i]);
  }
}

/**
 * () Closest Pair Calculation
 */
float dist(Point* a, Point* b) {
  return sqrt(pow(a->x - b->x, 2) + pow(a->y - b->y, 2));
}

float min(float a, float b) {
  return (a < b) ? a : b;
}

Pair* createPair(Point* a, Point* b, float distance) {
  Pair* pair = (Pair*) malloc(sizeof(Pair));
  pair->a = a;
  pair->b = b;
  pair->distance = distance;
  return pair;
}

// Generic function for retrieving closest pair below some specified distance.
// Also has capability to exit if distance in Y dimensions starts growing.
Pair* getClosestPairBelowBound(Space* space, float bound, bool exitIfYGrows) {
  Point** points = space->points;
  int count = space->count;
  float min = bound;
  int i, j;
  Point* a = NULL;
  Point* b = NULL;

  for (i = 0; i < count; i++) {
    for (j = i + 1; j < count && (!exitIfYGrows || ((points[j]->y - points[i]->y) < min)); j++) {
      float distance = dist(points[i], points[j]); 
      if (distance < min) {
        a = points[i];
        b = points[j];
        min = distance;
      }
    }
  }

  // Return null if no closest pair found below bound.
  if (a == NULL) return NULL;

  // Otherwise return pair.
  return createPair(a, b, min);
}

// Compare every point in space and findout closer pair.
Pair* getClosestPairByBruteForce(Space* space) {
  return getClosestPairBelowBound(space, FLT_MAX, false);
}

// Will create a strip-subspace with given midpoint and distance.
// Then will search closest pair in strip-subspace which are also
// closer than the given distance.
Pair* getClosestPairInStripSubspace(Space* spaceY, Point* midpoint, float distance) {
  // Create strip subspace which will contain points that
  // closer more than given distance to the midpoint.
  Space* subspace = createSpace(spaceY->count);
  int j = 0;
  int i;
  
  for (i = 0; i < spaceY->count; i++) {
    if (abs(spaceY->points[i]->x - midpoint->x) < distance) {
      subspace->points[j++] = spaceY->points[i];
    }
  }
  subspace->count = j; // fix the count.

  // Find closest pair of strip subspace.
  Pair* closestPair = getClosestPairBelowBound(subspace, distance, true);
  freeSpace(subspace);
  return closestPair;
}

// Recursive part of the closest pair calculation function. 
Pair* getClosestPair(Space* spaceX, Space* spaceY) {
  // Return pair directly if n == 2.
  if (spaceX->count == 2) {
    Point* a = spaceX->points[0];
    Point* b = spaceX->points[1];
    return createPair(a, b, dist(a, b));
  }

  // Do brute-force if n == 3
  if (spaceX->count == 3) {
    return getClosestPairByBruteForce(spaceX);
  }

  // Find middle point in x-axis.
  int count = spaceX->count;
  int mid = count / 2;
  Point* midpoint = spaceX->points[mid];
  int midX = midpoint->x;
  int midY = midpoint->y;

  // Create left and right SpaceX.
  Space* leftSpaceX = sliceSpace(spaceX, 0, mid);
  Space* rightSpaceX = sliceSpace(spaceX, mid, count);

  // Create left and right SpaceY.
  Space* leftSpaceY = createSpace(mid);
  Space* rightSpaceY = createSpace(count - mid);

  // Fill subspaces by splitting space vertically by mid point.
  int li = 0; // Index of left subspace.
  int ri = 0; // Index of right subspace.
  int i;
  for (i = 0; i < count; i++) {
    Point* p = spaceY->points[i];
    // If left subspace is not full AND
    // (point is to the left of midpoint OR
    // point is on the same x-axis but below midpoint)
    // Then add point to the left subspace.
    // Otherwise; add it to the right subspace.
    if (li < mid && (p->x < midX || (p->x == midX && p->y < midY))) {
      leftSpaceY->points[li++] = p;
    } else {
      rightSpaceY->points[ri++] = p;
    }
  }

  // Temporary closest pair pointer.
  Pair* closestPair;

  // Get closest pairs of left and right subspaces. 
  Pair* leftClosestPair = getClosestPair(leftSpaceX, leftSpaceY);
  Pair* rightClosestPair = getClosestPair(rightSpaceX, rightSpaceY);

  // Pick closest one of two pairs.
  if (leftClosestPair->distance <= rightClosestPair->distance) {
    closestPair = leftClosestPair;
    free(rightClosestPair);
  } else {
    closestPair = rightClosestPair;
    free(leftClosestPair);
  }

  // Get closest pair in strip space.
  Pair* closerPairInStripSpace = getClosestPairInStripSubspace(spaceY, midpoint, closestPair->distance);

  // Pick closest one again if strip space has even closest pair.
  if (closerPairInStripSpace != NULL) {
    if (closerPairInStripSpace->distance < closestPair->distance) {
      free(closestPair);
      closestPair = closerPairInStripSpace;
    } else {
      free(closerPairInStripSpace);
    }
  }

  // Free temporary subspaces.
  freeSpace(leftSpaceX);
  freeSpace(leftSpaceY);
  freeSpace(rightSpaceX);
  freeSpace(rightSpaceY);

  return closestPair;
}

// Main function to find closest pair.
Pair* findClosestPair(Space* space) {
  // Create two copy of space.
  Space* spaceX = copySpace(space);
  Space* spaceY = copySpace(space);

  // Sort by X and Y.
  sortSpace(spaceX, sortSpaceCompareX);
  sortSpace(spaceY, sortSpaceCompareY);

  // Get closest pair.
  Pair* closestPair = getClosestPair(spaceX, spaceY);

  // Free temporary spaces.
  freeSpace(spaceX);
  freeSpace(spaceY);

  return closestPair;
}

/**
 * () Main
 */
void printPair(Pair* pair) {
  printf(" Point A: (x:%d, y:%d)\n Point B: (x:%d, y:%d)\n Distance: %.3f\n",
    pair->a->x, pair->a->y, pair->b->x, pair->b->y, pair->distance);
}

int main(int argc, char** argv) {
  char* input = DEFAULT_INPUT;
  if (argc >= 2) input = argv[1];
  printf("[Processing file %s]\n", input);

  // Read input file and create Space.
  Space* space = createSpaceFromFile(input);

  // Find and print closest pair.
  Pair* closestPair = findClosestPair(space);
  printf("Result of Divide & Conquer Method: \n");
  printPair(closestPair);

  // Test of correctness, calculate with brute-force and compare.
  Pair* pairbf = getClosestPairByBruteForce(space);
  if (
    (pairbf->distance != closestPair->distance) || 
    ((pairbf->a != closestPair->a || pairbf->b != closestPair->b) &&
      (pairbf->b != closestPair->a || pairbf->a != closestPair->b))
  ) {
    printf("ALERT: Results of D&C and BF DO NOT MATCH!");
    printf("Result of Brute-Force Method: \n");
    printPair(pairbf);
  }
  free(pairbf);

  // Free space and the points it have.
  freePointsOfSpace(space);
  freeSpace(space);
  free(closestPair);

  return 0;
}
