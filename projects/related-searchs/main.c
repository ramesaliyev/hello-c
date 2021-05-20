#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"
#include "ctype.h"

#define LINESIZE 256
#define NULCHR '\0'
#define NLCHR '\n'
#define NLSTR "\n"
#define INDENT "    "

/**
 * Non-Logic-Specific Common functionalities.
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

int getNextAvailableSymbol(Graph* graph) {
  int i = 0;
  while (graph->symbols[i]->text != NULL) {
    i++;
  }
  return i;
}

int getSymbol(Graph* graph, char* text) {
  int i;
  for (i = 0; i < graph->vertexCount; i++) {
    char* symbolText = graph->symbols[i]->text;
    if (symbolText != NULL && strcmp(symbolText, text) == 0) {
      return i;
    }
  }

  return -1;
}

int getCreateSymbol(Graph* graph, char* text) {
  int symbol = getSymbol(graph, text);

  if (symbol == -1) {
    symbol = getNextAvailableSymbol(graph);
    graph->symbols[symbol]->text = mallocstr(LINESIZE);
    strcpy(graph->symbols[symbol]->text, text);
  }

  return symbol;
}

bool hasVertex(Graph* graph, char* text) {
  return getSymbol(graph, text) != -1;
}

void sortDescVerticesOfSymbol(Graph* graph, int symbol) {
  Vertex* currVertex = NULL;
  Vertex* nextVertex = NULL;
  Symbol* nextSymbol = NULL;

  currVertex = graph->adjList[symbol];

  while (currVertex != NULL) {
    nextVertex = currVertex->next;

    while (nextVertex != NULL) {
      if (currVertex->symbol->searchCount < nextVertex->symbol->searchCount) {
        nextSymbol = nextVertex->symbol;
        nextVertex->symbol = currVertex->symbol;
        currVertex->symbol = nextSymbol;
      }

      nextVertex = nextVertex->next;
    }

    currVertex = currVertex->next;
  }
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
 * Assigment Logic.
 */
Graph* buildGraphFromInput() {
  printf(INDENT"How many query graph has? (Vertex count?): ");
  int vertexCount = scanInt();
  
  if (vertexCount == 0) {
    printf(INDENT"Error: Cannot use 0 as vertex count, input ignored.\n");
    return buildGraphFromInput();
  }

  Graph* graph = createGraph(vertexCount);

  int i;
  for (i = 0; i < vertexCount; i++) {
    printf(INDENT"%d/%d Enter text of query #%d: ", i+1, vertexCount, i+1);
    char* text = scanLine();

    if (strcmp(text, "") == 0) {
      printf(INDENT"Error: Text cannot be empty string, input ignored.\n");
      i--;
    } else {
      if (!hasVertex(graph, text)) {
        graph->symbols[i]->text = text;
      } else {
        printf(INDENT"Error: Vertex with this query alread exist, input ignored.\n");
        i--;
      }
    }
  }

  printf("\n");
  printf(INDENT"Please mark similar queries by providing comma separated query ids.\n");
  printf(INDENT"For example if query #1 similar to #2 enter 1-2 and press enter.\n");
  printf(INDENT"If you enter for example 1-2 do not enter 2-1 since it is same relation.\n");
  printf(INDENT"When you done enter empty string to finish.\n\n");
  
  while (true) {
    printf(INDENT"Enter relation: ");
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
            printf(INDENT"Error: Relation exist, input ignored.\n");
          }
        } else {
          printf(INDENT"Error: Wrong input, ids out of bound, input ignored.\n");
        }       
      } else {
        printf(INDENT"Error: Wrong formatted input, ignored.\n");
      }
    } else {
      break;
    }

    free(line);
  }

  return graph;
}

Graph* mergeGraphs(Graph* graphA, Graph* graphB) {
  int totalVertexCount = graphA->vertexCount;

  int i;
  for (i = 0; i < graphB->vertexCount; i++) {
    if (!hasVertex(graphA, graphB->symbols[i]->text)) {
      totalVertexCount++;
    }
  }

  Graph* mergedGraph = createGraph(totalVertexCount);

  Graph* graphs[] = {graphA, graphB};
  int j;
  for (j = 0; j < 2; j++) {
    Graph* graph = graphs[j];
    int k;
    for (k = 0; k < graph->vertexCount; k++) {
      int src = getCreateSymbol(mergedGraph, graph->symbols[k]->text);

      Vertex* vertex = graph->adjList[k];
      while (vertex != NULL) {
        int dest = getCreateSymbol(mergedGraph, vertex->symbol->text);
        
        if (!hasEdge(mergedGraph, src, dest)) {
          addEdge(mergedGraph, src, dest);
        }
        
        vertex = vertex->next;
      }
    }
  } 

  return mergedGraph;
}

void showMostRelatedQueries(Graph* graph, char* query) {
  int resultLength = 3;
  int symbol = getSymbol(graph, query);

  if (symbol == -1) {
    printf(INDENT"Given query does not exist in database.\n");
  } else {
    sortDescVerticesOfSymbol(graph, symbol);

    Vertex* vertex = graph->adjList[symbol];

    if (vertex == NULL) {
      printf(INDENT"Given query has no related queries.\n");
    } else {
      int i = 0;
      printf(INDENT"Related queries:\n");

      while (i < resultLength && vertex != NULL) {
        printf(INDENT"- %s (Search count: %d) \n", vertex->symbol->text, vertex->symbol->searchCount);
        vertex = vertex->next;
        i++;
      }
    }

    graph->symbols[symbol]->searchCount++;
  }
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
  Graph* mergedGraph = mergeGraphs(graphA, graphB);

  printf("\nFirst Graph: \n");
  printGraph(graphA);
  printf("\nSecond Graph: \n");
  printGraph(graphB);
  printf("\nMerged Graph: \n");
  printGraph(mergedGraph);

  // Step 3: Query graphs.
  printf("\nSearch engine ready for queries!\n");
  printf("Enter a query to see it's most related queries: \n");
  printf("Enter \"exit\" to exit and \"print\" to print graph.\n");

  while (true) {
    printf("\n");
    printf(INDENT"Enter query: ");
    char* query = scanLine();

    if (strcmp(query, "exit") == 0) {
      break;
    } else if (strcmp(query, "print") == 0) {
      printf("\nMerged Graph: \n");
      printGraph(mergedGraph);
    } else {
      showMostRelatedQueries(mergedGraph, query);
      printf("\n");
    }
  }  

  freeGraph(graphA);
  freeGraph(graphB);
  freeGraph(mergedGraph);

  return 0;
}
