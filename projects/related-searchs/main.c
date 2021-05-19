#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"
#include "ctype.h"

#define LINESIZE 256
#define NULCHR '\0'
#define NLCHR '\n'
#define NLSTR "\n"
#define I1 "    "

/**
 * Data structure operations.
 */
typedef struct Vertex Vertex;
typedef struct Graph Graph;
typedef struct Symbol Symbol;

struct Graph {
  int vertexCount;
  Vertex** adjList;
  Symbol** symbols;
};

struct Vertex {
  Symbol* symbol;
  Vertex* next;
};

struct Symbol {
  int symbol;
  int searchCount;
  char* text;
};

Symbol* createSymbol(int symbolNo) {
  Symbol* symbol = (Symbol*) malloc(sizeof(Symbol));
  symbol->symbol = symbolNo;
  symbol->searchCount = 0;
  symbol->text = NULL;
  return symbol;
}

Vertex* createVertex(Symbol* symbol) {
  Vertex* vertex = (Vertex*) malloc(sizeof(Vertex));
  vertex->symbol = symbol;
  vertex->next = NULL;
  return vertex;
}

Graph* createGraph(int vertexCount) {
  Graph* graph = (Graph*) malloc(sizeof(Graph));
  graph->vertexCount = vertexCount;
  graph->adjList = malloc(vertexCount * sizeof(Vertex));
  graph->symbols = malloc(vertexCount * sizeof(Symbol));

  int i;
  for (i = 0; i < vertexCount; i++) {
    graph->symbols[i] = createSymbol(i);
    graph->adjList[i] = NULL;
  }

  return graph;
}

void addEdge(Graph* graph, int src, int dest) {
  Vertex* srcToDestVertex = createVertex(graph->symbols[dest]);
  srcToDestVertex->next = graph->adjList[src];
  graph->adjList[src] = srcToDestVertex;

  Vertex* destToSrcVertex = createVertex(graph->symbols[src]);
  destToSrcVertex->next = graph->adjList[dest];
  graph->adjList[dest] = destToSrcVertex;
}

bool hasEdge(Graph* graph, int src, int dest) {
  Vertex* vertex = graph->adjList[src];
  while (vertex != NULL) {
    if (vertex->symbol->symbol == dest) {
      return true;
    }

    vertex = vertex->next;
  }

  return false;
}

bool hasVertex(Graph* graph, char* text) {
  int i;
  for (i = 0; i < graph->vertexCount; i++) {
    char* symbolText = graph->symbols[i]->text;
    if (symbolText != null && strcmp(symbolText, text) == 0) {
      return true;
    }
  }

  return false;
}

void freeGraph(Graph* graph) {
  int i;
  for (i = 0; i < graph->vertexCount; i++) {
    Vertex* vertex = graph->adjList[i];
    while (vertex != NULL) {
      Vertex* nextVertex = vertex->next;
      free(vertex);
      vertex = nextVertex;
    }

    free(graph->symbols[i]->text);
    free(graph->symbols[i]);
  }

  free(graph);
}

void printGraph(Graph* graph) {
  int vertexCount = graph->vertexCount;
  Symbol** symbols = graph->symbols;

  int i;
  for (i = 0; i < vertexCount; i++) {
    printf("%s: (", symbols[i]->text);

    Vertex* vertex = graph->adjList[i];
    while (vertex != NULL) {
      printf("%s", vertex->symbol->text);
      vertex = vertex->next;

      if (vertex != NULL) {
        printf(", ");
      }
    }

    printf(")\n");
  }
}

/**
 * Common functionalities.
 */
bool inArrayChr(char* array, int size, char value) {
  int i = 0;
  while(i < size) if (array[i++] == value) return true;
  return false;
}

char* mallocstr(int size) {
  return (char*) malloc((size + 1) * sizeof(char));
}

void clearstr(char* str) {
  char allowed[] = {'-', ' ', '.', '?', ',', '!'};
  size_t allowedlen = sizeof(allowed) / sizeof(char);

  int i, j;
  char chr;

  i = j = 0;
  while ((chr = str[i++]) != NULCHR) {
    if (isalnum(chr) || inArrayChr(allowed, allowedlen, chr)) {
      str[j++] = chr;
    }
  }

  str[j] = NULCHR;
}

char* scanRaw(int size) {
  char* input = mallocstr(size);
  fgets(input, size, stdin);

  if (input[strlen(input)-1] != NLCHR) {
    int ch;
    while (((ch = getchar()) != NLCHR) && (ch != EOF));
  }

  return input;
}

char* scanStr(int size) {
  char* input = scanRaw(size);

  if (strcmp(input, NLSTR) == 0) {
    strcpy(input, "");
  } else if (strcmp(input, " \n") == 0) {
    strcpy(input, "");
  } else {
    clearstr(input);
  }

  return input;
}

int scanInt() {
  char* input = scanRaw(10);
  int num = atoi(input);
  free(input); 
  return num;
}

char* scanLine() {
  return scanStr(LINESIZE);
}

/**
 * Assigment Logic.
 */
Graph* buildGraphFromInput() {
  printf(I1"How many query graph has? (Vertex count?): ");
  int vertexCount = scanInt();
  
  Graph* graph = createGraph(vertexCount);

  int i;
  for (i = 0; i < vertexCount; i++) {
    printf(I1"%d/%d Enter text of query #%d: ", i+1, vertexCount, i+1);
    char* text = scanLine();

    if (!hasVertex(graph, text)) {
      graph->symbols[i]->text = text;
    } else {
      printf(I1"Error: Wrong formatted input, ignored.\n");
    }
  }

  printf("\n");
  printf(I1"Please mark similar queries by providing comma separated query ids.\n");
  printf(I1"For example if query #1 similar to #2 enter 1-2 and press enter.\n");
  printf(I1"If you enter for example 1-2 do not enter 2-1 since it is same relation.\n");
  printf(I1"When you done enter empty string to finish.\n\n");
  
  while (true) {
    printf(I1"Enter relation: ");
    char* line = scanLine();

    if (strcmp(line, "") != 0) {
      char* srcIdInput = strtok(line, "-");
      char* destIdInput = strtok(NULL, "-");

      if (srcIdInput != NULL && destIdInput != NULL) {
        int srcId = atoi(srcIdInput);
        int destId = atoi(destIdInput);

        if (srcId > 0 && destId > 0 && srcId <= vertexCount && destId <= vertexCount) {
          int src = srcId - 1;
          int dest = destId - 1;
          if (!hasEdge(graph, src, dest)) {
            addEdge(graph, src, dest);
          } else {
            printf(I1"Error: Relation exist, input ignored.\n");
          }
        } else {
          printf(I1"Error: Wrong input, ids out of bound, input ignored.\n");
        }       
      } else {
        printf(I1"Error: Wrong formatted input, ignored.\n");
      }
    } else {
      break;
    }

    free(line);
  }

  return graph;
}

Graph* mergeGraphs(Graph* graphA, Graph* graphB) {
}

/**
 * Main Flow
 */
int main() {
  printf("Welcome to YTU SearchIt 2021\n\n");
  
  // Step 1: Collect graphs.
  printf("1/2 Please enter asked information about first query graph.\n");
  Graph* graphA = buildGraphFromInput();

  printf("\n");
  printf("2/2 Please enter asked information about second query graph.\n");
  Graph* graphB = buildGraphFromInput();

  // Step 2: Merge graphs.
  Graph* mergedGraph = mergedGraphs(graphA, graphB);

  // Step 3: Query graphs.

  printf("Graph A: \n");
  printGraph(graphA);
  printf("Graph B: \n");
  printGraph(graphB);
  printf("Merged Graph: \n");
  printGraph(mergedGraph);

  freeGraph(graphA);
  freeGraph(graphB);
  freeGraph(mergedGraph);

  return 0;
}
