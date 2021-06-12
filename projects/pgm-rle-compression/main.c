#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

/**
 * README
 * 
 * - Both P5 and P2 format supported as input.
 * - Output file of decompressed PGM will be in it's original format.
 *   For example if your original PGM was P2, after compressing and 
 *   decompressing output PGM will be in P2 format (and vice-versa). 
 * - Encoded PGM files (CPGMs) has their own format in binary.
 * 
 * Code structure:
 * Search like (x) to jump directly to section.
 * 
 * (0) Program configuration definitions.
 * (1) Data types and structures.
 * (2) Common generic utilities.
 * (3) PGM related functions.
 * (4) Compress / Decompress
 * (5) Operations on compressed data.
 * (6) Interface operations
 * (7) Main
 */

/**
 * (0) Program configuration definitions.
 */
#define LINESIZE 256
#define NLCHR '\n'
#define NLSTR "\n"
#define MAX_COLOR 255
#define P5 "P5"
#define P2 "P2"
#define CPGM_TYPE "CPGMv0"
#define COMMENT_IDENTIFIER '#'
#define BLOCK_PER_ENTRY 3

/**
 * (1) Data types and structures.
 */
typedef uint16_t RunLength;
typedef uint8_t Pixel;
typedef uint8_t Block;
typedef struct PGM PGM;
typedef struct CPGM CPGM;
typedef struct Entry Entry;
typedef struct EntryNode EntryNode;
typedef struct ProcessorInput ProcessorInput;

typedef EntryNode* (*SequentialProcessor)(ProcessorInput*, void*);

struct PGM {
  char* type;
  Pixel* pixels;
  int maxValue;
  int width;
  int height;
};

struct CPGM {
  char* pgmType;
  Block* blocks;
  int blockCount;
  int entryCount;
  int maxValue;
  int width;
  int height;
};

struct Entry {
  RunLength runlength;
  Pixel pixel;
};

struct EntryNode {
  Entry* entry;
  EntryNode* next;
  EntryNode* prev;
};

struct ProcessorInput {
  Entry* entry;
  int prevRunLength;
};

/**
 * (2) Common generic utilities.
 */
char* mallocstr(int size) {
  return (char*) malloc((size + 1) * sizeof(char));
}

char* copystr(char* str) {
  char* copy = mallocstr(strlen(str));
  if (copy) strcpy(copy, str);
  return copy;
}

int max(int a, int b) {
  return a > b ? a : b;
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
 * (3) PGM related functions. 
 */
int skipWhitespace(FILE* file) {
  int chr;
  while ((chr = fgetc(file)) != EOF && isspace(chr));
  if (chr != EOF) fseek(file, -1, SEEK_CUR);
  return chr;
}

void skipCommentsAndWhitespace(FILE* file) {
  int nextChr = skipWhitespace(file);

  // Go one byte back if not comment.
  if (nextChr == COMMENT_IDENTIFIER) {
    // Skip line.
    char line[LINESIZE];
    fgets(line, sizeof(line), file);
      
    // Continue to skipping next comments.
    skipCommentsAndWhitespace(file);
  }
}

PGM* createPGM(char* type, int maxValue, int width, int height) {
  PGM* pgm = (PGM*) malloc(sizeof(PGM));

  if (pgm == NULL) {
    printf("Error: Could not allocate memory for PGM.");
    return NULL;
  }

  pgm->pixels = (Pixel*) calloc(width * height, sizeof(Pixel));

  if (pgm->pixels == NULL) {
    printf("Error: Could not allocate memory for PGM pixels.");
    return NULL;
  }

  pgm->type = copystr(type);
  pgm->maxValue = MAX_COLOR; //maxValue;
  pgm->width = width;
  pgm->height = height;

  return pgm;
}

PGM* readPGM(char* filepath) {
  FILE* file = openFile(filepath, "rb");
  
  if (file == NULL) {
    printf("Error: Cannot find PGM file in given path: %s\n", filepath);
    return NULL;
  }
  
  char pgmType[LINESIZE];
  int maxValue;
  int width;
  int height;

  skipCommentsAndWhitespace(file);
  fscanf(file, "%s", pgmType);

  if (strcmp(pgmType, P5) && strcmp(pgmType, P2)) {
    printf("Error: Incorrect PGM type, it should be either "P5" or "P2". Given type: %s\n", pgmType);
    return NULL;
  }
  
  skipCommentsAndWhitespace(file);
  fscanf(file, "%d %d", &(width), &(height));
  skipCommentsAndWhitespace(file);
  fscanf(file, "%d", &(maxValue));
  fgetc(file); // Skip last whitespace before pixels.

  PGM* pgm = createPGM(pgmType, maxValue, width, height);

  if (pgm == NULL) {
    printf("Error: Cannot read PGM due to memory allocation issues.\n");
    return NULL;
  }

  int pixelCount = pgm->width * pgm->height;

  // Read pixel values into pgm.
  if (!strcmp(pgmType, P5)) {
    // Read P5
    fread(pgm->pixels, sizeof(Pixel), pixelCount, file);
  } else {
    // Read P2
    int i, color;
    for (i = 0; i < pixelCount; i++) {
      skipWhitespace(file);
      fscanf(file, "%d", &(color));
      pgm->pixels[i] = color;

      if (color < 0 || color > pgm->maxValue) {
        printf("Error: Out of bound (0~%d) value %d read as color from PGM.\n", pgm->maxValue, color);
        return NULL;        
      }
    }
  }

  fclose(file);

  return pgm;
}

void writePGMWithType(PGM* pgm, char* filepath, char* type) {
  FILE* file = openFile(filepath, "wb");
  
  // Write header
  fprintf(file, 
    "%s\n%d %d\n%d\n",
    type,
    pgm->width,
    pgm->height,
    pgm->maxValue
  );
  
  int pixelCount = pgm->width * pgm->height;

  // Write pixels.
  if (!strcmp(type, P5)) {
    // Write P5
    fwrite(pgm->pixels, sizeof(Pixel), pixelCount, file);
  } else {
    // Write P2
    int i;
    for (i = 0; i < pixelCount; i++) {
      fprintf(file, "%d%c", pgm->pixels[i], ((i+1) % pgm->width) ? ' ' : '\n');
    }
  }
  
  fclose(file);
}

void writePGM(PGM* pgm, char* filepath) {
  writePGMWithType(pgm, filepath, pgm->type);
}

void freePGM(PGM* pgm) {
  free(pgm->type);
  free(pgm->pixels);
  free(pgm);
}

/**
 * (4) Compress / Decompress
 */
Entry* createEntry(Pixel pixel) {
  Entry* entry = (Entry*) malloc(sizeof(Entry));
  entry->runlength = 0;
  entry->pixel = pixel;
  return entry;
}

void updateEntry(Entry* entry, CPGM* cpgm, int offset) {
  int index = offset * BLOCK_PER_ENTRY; 
  Block rlLSB = cpgm->blocks[index];
  Block rlMSB = cpgm->blocks[index + 1];
  entry->pixel = cpgm->blocks[index + 2];
  entry->runlength = (rlMSB << 8) | rlLSB;
}

void freeEntry(Entry* entry) {
  free(entry);
}

void insertBlock(Block* blocks, RunLength runlength, Pixel pixel, int offset) {
  int index = offset * BLOCK_PER_ENTRY; 
  Block rlLSB = runlength & 0xFF;
  Block rlMSB = runlength >> 8;
  blocks[index] = rlLSB;
  blocks[index + 1] = rlMSB;
  blocks[index + 2] = pixel;
}

bool validateCPGM(CPGM* cpgm) {
  Entry* entry = createEntry(0);

  int j;
  int pixelCount = 0;
  int prevColor = -1;

  for (j = 0; j < cpgm->entryCount; j++) {
    updateEntry(entry, cpgm, j);
    pixelCount += entry->runlength;

    if (entry->pixel < 0 || entry->pixel > cpgm->maxValue) {
      printf("Error: Color %d is out of bound of CPGM color range (0~%d).\n", entry->pixel, cpgm->maxValue);
      return false;
    }

    if (prevColor == entry->pixel) {
      printf("Error: Color %d repeating in CPGM encoding.\n", entry->pixel);
      return false;
    }

    prevColor = entry->pixel;
  }

  if (pixelCount != cpgm->width * cpgm->height) {
    printf("Error: Pixel count of CPGM does not match with width*height.\n");
    return false;
  }

  freeEntry(entry);
  return true;
}

CPGM* createCPGM(int blockCount, char* pgmType, int maxValue, int width, int height) {
  CPGM* cpgm = (CPGM*) malloc(sizeof(CPGM));

  if (cpgm == NULL) {
    printf("Error: Could not allocate memory for CPGM.");
    return NULL;
  }

  cpgm->blocks = (Block*) calloc(BLOCK_PER_ENTRY * blockCount, sizeof(Block));

  if (cpgm->blocks == NULL) {
    printf("Error: Could not allocate memory for CPGM blocks.");
    return NULL;
  }

  cpgm->pgmType = copystr(pgmType);
  cpgm->blockCount = blockCount;
  cpgm->entryCount = blockCount / BLOCK_PER_ENTRY;
  cpgm->maxValue = maxValue;
  cpgm->width = width;
  cpgm->height = height;

  return cpgm;
}

CPGM* compressPGM(PGM* pgm) {
  int pixelCount = pgm->width * pgm->height;
  int entryCapacity = 16;
  int entryCount = 0;
  
  Block* blocks = (Block *) calloc(BLOCK_PER_ENTRY * entryCapacity, sizeof(Block));
  Pixel pixel = pgm->pixels[0];
  RunLength runlength = 0;

  int i;
  for (i = 0; i < pixelCount; i++) {
    // Resize if needed.
    if (entryCount == entryCapacity) {
      entryCapacity *= 2;
      blocks = (Block *) realloc(blocks, BLOCK_PER_ENTRY * entryCapacity * sizeof(Block));
    }

    Pixel nextPixel = pgm->pixels[i];

    if (nextPixel != pixel) {
      insertBlock(blocks, runlength, pixel, entryCount);

      runlength = 0;
      pixel = nextPixel;
      entryCount++;
    }

    runlength++;
  }

  if (runlength > 0) {
    insertBlock(blocks, runlength, pixel, entryCount);
    entryCount++;
  }

  CPGM* cpgm = createCPGM(BLOCK_PER_ENTRY * entryCount, pgm->type, pgm->maxValue, pgm->width, pgm->height);

  if (cpgm == NULL) {
    printf("Error: Could not compress PGM due to memory issues.");
    return NULL;
  }

  cpgm->blocks = blocks;

  return cpgm;
}

PGM* decompressCPGM(CPGM* cpgm) {
  if (!validateCPGM(cpgm)) {
    printf("Error: Given CPGM is not valid state.\n");
    return NULL;
  }
  
  Entry* entry = createEntry(0);
  PGM* pgm = createPGM(cpgm->pgmType, cpgm->maxValue, cpgm->width, cpgm->height);

  int prevRunLength = 0;
  int j;
  for (j = 0; j < cpgm->entryCount; j++) {
    updateEntry(entry, cpgm, j);
    memset(pgm->pixels + prevRunLength, entry->pixel, entry->runlength);
    prevRunLength += entry->runlength;
  }

  freeEntry(entry);

  return pgm;
}

CPGM* readCPGM(char* filepath) {
  FILE* file = openFile(filepath, "rb");

  if (file == NULL) {
    printf("Error: Cannot find CPGM file in given path: %s\n", filepath);
    return NULL;
  }

  char cpgmType[LINESIZE];
  char pgmType[LINESIZE];
  int blockCount;
  int maxValue;
  int width;
  int height;

  skipCommentsAndWhitespace(file);
  fscanf(file, "%s", cpgmType);

  if (strcmp(cpgmType, CPGM_TYPE)) {
    printf("Error: Incorrect CPGM type, it should be "CPGM_TYPE". Given type: %s\n", cpgmType);
    return NULL;
  }

  skipCommentsAndWhitespace(file);
  fscanf(file, "%s", pgmType);
  skipCommentsAndWhitespace(file);
  fscanf(file, "%d %d %d %d", &(width), &(height), &(maxValue), &(blockCount));
  fgetc(file); // Skip last whitespace before pixels.

  CPGM* cpgm = createCPGM(blockCount, pgmType, maxValue, width, height);

  if (cpgm == NULL) {
    printf("Error: Cannot read CPGM due to memory allocation issues.");
    return NULL;
  }

  // Read pixel values into pgm.
  fread(cpgm->blocks, sizeof(Block), cpgm->blockCount, file);
  fclose(file);

  return cpgm;
}

void writeCPGM(CPGM* cpgm, char* filepath) {
  FILE* file = openFile(filepath, "wb");
  
  // Write header
  fprintf(file, 
    CPGM_TYPE"\n%s\n%d %d %d %d\n",
    cpgm->pgmType,
    cpgm->width,
    cpgm->height,
    cpgm->maxValue,
    cpgm->blockCount
  );
  
  // Write blocks.
  fwrite(cpgm->blocks, sizeof(Block), cpgm->blockCount, file);
  
  fclose(file);
}

void freeCPGM(CPGM* cpgm) {
  free(cpgm->pgmType);
  free(cpgm->blocks);
  free(cpgm);
}

/**
 * (5) Operations on compressed file.
 */
EntryNode* createEntryNode(Pixel pixel) {
  EntryNode* entryList = (EntryNode*) malloc(sizeof(EntryNode));
  entryList->entry = createEntry(pixel);
  entryList->entry->runlength = 0;
  entryList->next = NULL;
  entryList->prev = NULL;
  return entryList;
}

EntryNode* createEntryList(Pixel pixel) {
  EntryNode* head = createEntryNode(pixel);
  head->next = createEntryNode(pixel);
  head->next->next = createEntryNode(pixel);
  return head;
}

void printEntryList(EntryNode* head) {
  EntryNode* curr = head;

  while (curr != NULL) {
    printf("(%d,%d)\n", curr->entry->runlength, curr->entry->pixel);
    curr = curr->next;
  }
}

void freeEntryNode(EntryNode* entryNode) {
  freeEntry(entryNode->entry);
  free(entryNode);
}

void freeEntryList(EntryNode* entryNode) {
  EntryNode* tmp;
  while((tmp = entryNode->next) != NULL) {
    freeEntryNode(entryNode);
    entryNode = tmp;
  }
  freeEntryNode(entryNode);
}

void entryProcessor(ProcessorInput* input, CPGM* cpgm, int offset, SequentialProcessor processor, void* data) {
  Entry* entry = input->entry;

  // Get entry.
  updateEntry(entry, cpgm, offset);

  // Pass current node to processor.
  EntryNode* updateList = processor(input, data);

  // Should we update?
  if (updateList != NULL) {
    int blockCount = cpgm->blockCount;
    int entryCount = cpgm->entryCount;
    int affectedEntryCount = 1;
    int finalEntryCount = 1;

    // Create full list.
    EntryNode* head = updateList;
    EntryNode* tmp;

    // If not first node, get previous.
    if (offset - 1 >= 0) {
      head = createEntryNode(0);
      head->next = updateList;
      updateEntry(head->entry, cpgm, offset - 1);
      affectedEntryCount++;
    }

    // If not last node, get next.
    if (offset + 1 < entryCount) {
      EntryNode* last = head;
      while(last->next != NULL) last = last->next;
      last->next = createEntryNode(0);
      updateEntry(last->next->entry, cpgm, offset + 1);
      affectedEntryCount++;
    }

    // Now we'll clear nodes of repeating colors and
    // ones that has zero runlength.
    tmp = head;

    // Clear & merge head node.
    if (tmp->entry->runlength == 0) {
      tmp = tmp->next; 
      freeEntryNode(head);
      head = tmp;
    }

    // Clear & merge the rest, and count final entry count.
    while (tmp->next != NULL) {
      EntryNode* next = tmp->next;

      if (next->entry->runlength == 0) {
        tmp->next = next->next;
        freeEntryNode(next);
      } else if (tmp->entry->pixel == next->entry->pixel) {
        tmp->entry->runlength += next->entry->runlength;
        tmp->next = next->next;
        freeEntryNode(next);
      } else {
        finalEntryCount++;
        tmp = tmp->next;
      }
    }

    // Now we will insert updated blocks in place.
    int nextEntryCount = entryCount - affectedEntryCount + finalEntryCount;
    int nextBlockCount = nextEntryCount * BLOCK_PER_ENTRY;
    int updateOffset = max(0, offset - 1);

    // Resize blocks if needed.
    if (entryCount != nextEntryCount) {
      Block* blocks = (Block*) calloc(nextBlockCount, sizeof(Block));

      // Do index calculations.
      int leftEnd = updateOffset * BLOCK_PER_ENTRY;
      int targetRightStart = leftEnd + (BLOCK_PER_ENTRY * finalEntryCount);
      int sourceRightStart = leftEnd + (BLOCK_PER_ENTRY * affectedEntryCount);

      // Copy left and right parts of affected data.
      memmove(blocks, cpgm->blocks, leftEnd);
      memmove(blocks + targetRightStart, cpgm->blocks + sourceRightStart, blockCount - sourceRightStart);

      // Replace source.
      free(cpgm->blocks);
      cpgm->blocks = blocks;
      cpgm->blockCount = nextBlockCount;
      cpgm->entryCount = nextEntryCount;
    }

    // Insert new data in place.
    tmp = head;
    while (tmp != NULL) {
      insertBlock(cpgm->blocks, tmp->entry->runlength, tmp->entry->pixel, updateOffset);
      tmp = tmp->next;
      updateOffset++;
    }

    // Clear and continue.
    freeEntryList(head);
    offset = --updateOffset; // take last (wrong) increment back.
  }

  // Continue with next node.
  if (offset < cpgm->entryCount - 1) {
    input->prevRunLength += entry->runlength;
    entryProcessor(input, cpgm, ++offset, processor, data);
  }
}

void processEntries(CPGM* cpgm, SequentialProcessor processor, void* data) {
  ProcessorInput* input = (ProcessorInput*) malloc(sizeof(ProcessorInput));
  input->prevRunLength = 0;
  input->entry = createEntry(0);

  entryProcessor(input, cpgm, 0, processor, data);
  freeEntry(input->entry);
  free(input);
}

EntryNode* replaceColorProcessor(ProcessorInput* input, void* args) {
  Pixel* colors = (Pixel*) args;
  Pixel prevColor = colors[0];
  Pixel nextColor = colors[1];
  
  Entry* entry = input->entry;
  if (entry->pixel == prevColor) {
    EntryNode* node = createEntryNode(nextColor);
    node->entry->runlength = entry->runlength;
    return node;
  }

  return NULL;
}

void replaceColor(CPGM* cpgm, Pixel prevColor, Pixel nextColor) {
  Pixel args[] = {prevColor, nextColor};
  processEntries(cpgm, replaceColorProcessor, args);
}

EntryNode* setColorProcessor(ProcessorInput* input, void* data) {
  int* args = (int *) data;
  int position = args[0];
  int newColor = args[1]; 

  Entry* entry = input->entry;
  int leftBound = input->prevRunLength;
  int rightBound = leftBound + entry->runlength;

  // Position is in this block.
  if (position > leftBound && position <= rightBound) {
    // Create new list.
    EntryNode* prev = createEntryList(entry->pixel);
    EntryNode* node = prev->next;
    EntryNode* next = node->next;

    // Set color.
    node->entry->pixel = newColor;

    // Adjust runlengths.
    int index = position - input->prevRunLength;
    prev->entry->runlength = index - 1;
    node->entry->runlength = 1;
    next->entry->runlength = entry->runlength - index;

    return prev;
  }

  return NULL;
}

void setColor(CPGM* cpgm, int row, int column, Pixel color) {
  int pos = (row * cpgm->width) + column + 1;
  int args[] = {pos, color};
  processEntries(cpgm, setColorProcessor, args);
}

void printHistogram(CPGM* cpgm) {
  RunLength colors[MAX_COLOR + 1] = {0};
  Entry* entry = createEntry(0);

  int j;
  for (j = 0; j < cpgm->entryCount; j++) {
    updateEntry(entry, cpgm, j);
    colors[entry->pixel] += entry->runlength;
  }

  printf("%-5s  %-5s\n", "COLOR", "COUNT");
  printf("-----  ---------------\n");

  int total = 0;
  int k;
  for (k = 0; k <= MAX_COLOR; k++) {
    RunLength count = colors[k];

    if (count > 0) {
      printf("%-5d  %-5hu\n", k, count);
      total++;
    }
  }

  printf("----------------------\n");
  printf("Total color count: %3d\n", total);
  printf("----------------------\n");

  freeEntry(entry);
}

/**
 * (6) Interface operations
 */
char* scanLine() {
  char* input = mallocstr(LINESIZE);
  fgets(input, LINESIZE, stdin);

  if (input[strlen(input)-1] != NLCHR) {
    int ch;
    while (((ch = getchar()) != NLCHR) && (ch != EOF));
  }

  if (strcmp(input, NLSTR) == 0) {
    strcpy(input, "");
  } else if (strcmp(input, " \n") == 0) {
    strcpy(input, "");
  } else {
    input[strcspn(input, "\r\n")] = 0;
  }

  return input;
}

void help() {
  printf("---------------------------------------------------------------------------------------------------------------\n");
  printf("$ %-45s %s\n", "compress <input.pgm> [<output.cpgm>]", "- will compress pgm to cpgm (default output = text_encoded.txt)");
  printf("$ %-45s %s\n", "decompress <input.cpgm> [<output.pgm>]", "- will compress cpgm to pgm (default output = text_decoded.pgm)");
  printf("$ %-45s %s\n", "replacecolor <prev> <next> <input.cpgm>", "- will replace all <prev> colors with <next> color");
  printf("$ %-45s %s\n", "setcolor <row> <column> <color> <input.cpgm>", "- will change color of position with given <color>");
  printf("$ %-45s %s\n", "histogram <input.cpgm>", "- show histogram of given cpgm");
  printf("$ %-45s %s\n", "convert <format> <input.pgm> <output.pgm>", "- will convert pgm file to given <format> (P2 or P5)");
  printf("$ %-45s %s\n", "help", "- display this message");
  printf("$ %-45s %s\n", "exit", "- exit from program");
  printf("---------------------------------------------------------------------------------------------------------------\n");
}

void print_incorrect_args() {
  printf("Error: Incorrect arguments, please check your arguments, type 'help' to see usages!\n");
}

void menu_compress() {
  char* input = strtok(NULL, " ");
  char* output = strtok(NULL, " ");
  
  if (input == NULL) {
    print_incorrect_args();
    return;
  }

  if (output == NULL) {
    output = "text_encoded.txt";
  }

  PGM* pgm = readPGM(input);
  if (pgm == NULL) return;
  CPGM* cpgm = compressPGM(pgm);
  if (cpgm == NULL) return;

  writeCPGM(cpgm, output);
  freeCPGM(cpgm);
  freePGM(pgm);

  printf("-> %s successfully compressed and saved to %s.\n", input, output);
}

void menu_decompress() {
  char* input = strtok(NULL, " ");
  char* output = strtok(NULL, " ");

  if (input == NULL) {
    print_incorrect_args();
    return;
  }

  if (output == NULL) {
    output = "text_decoded.pgm";
  }

  CPGM* cpgm = readCPGM(input);
  if (cpgm == NULL) return;
  PGM* pgm = decompressCPGM(cpgm);
  if (pgm == NULL) return;

  writePGM(pgm, output);
  freeCPGM(cpgm);
  freePGM(pgm);

  printf("-> %s successfully decompressed and saved to %s.\n", input, output);
}

void menu_replacecolor() {
  char* prev = strtok(NULL, " ");
  char* next = strtok(NULL, " ");
  char* input = strtok(NULL, " ");

  if (prev == NULL || next == NULL || input == NULL) {
    print_incorrect_args();
    return;
  }

  int prevColor = atoi(prev);
  int nextColor = atoi(next);

  CPGM* cpgm = readCPGM(input);
  if (cpgm == NULL) return;

  if (prevColor < 0 || prevColor > cpgm->maxValue || nextColor < 0 || nextColor > cpgm->maxValue) {
    printf("Error: Given color values are out of bounds (0~%d) of PGM.\n", cpgm->maxValue);
    freeCPGM(cpgm);
    return;
  }

  replaceColor(cpgm, prevColor, nextColor);

  writeCPGM(cpgm, input);
  freeCPGM(cpgm);

  printf("-> %d colors in %s successfully replaced with %d.\n", prevColor, input, nextColor);
}

void menu_setcolor() {
  char* row = strtok(NULL, " ");
  char* column = strtok(NULL, " ");
  char* color = strtok(NULL, " ");
  char* input = strtok(NULL, " ");

  if (row == NULL || column == NULL || color == NULL || input == NULL) {
    print_incorrect_args();
    return;
  }

  int rowInt = atoi(row);
  int columnInt = atoi(column);
  int colorInt = atoi(color);

  CPGM* cpgm = readCPGM(input);
  if (cpgm == NULL) return;

  if (colorInt < 0 || colorInt > cpgm->maxValue) {
    printf("Error: Given color value are out of bounds (0~%d) of PGM.\n", cpgm->maxValue);
    freeCPGM(cpgm);
    return;
  }

  if (rowInt >= cpgm->width || columnInt >= cpgm->height || rowInt < 0 || columnInt < 0) {
    printf("Error: Given coordinates are out of dimensions (%dx%d) of PGM.\n", cpgm->width, cpgm->height);
    freeCPGM(cpgm);
    return;
  }

  setColor(cpgm, rowInt, columnInt, colorInt);

  writeCPGM(cpgm, input);
  freeCPGM(cpgm);

  printf("-> Color at row=%d column=%d successfully set as %d in %s.\n", rowInt, columnInt, colorInt, input);
}

void menu_histogram() {
  char* input = strtok(NULL, " ");

  if (input == NULL) {
    print_incorrect_args();
    return;
  }

  CPGM* cpgm = readCPGM(input);
  if (cpgm == NULL) return;

  printHistogram(cpgm);
  freeCPGM(cpgm);
}

void menu_convert() {
  char* type = strtok(NULL, " ");
  char* input = strtok(NULL, " ");
  char* output = strtok(NULL, " ");

  if (type == NULL || input == NULL || output == NULL) {
    print_incorrect_args();
    return;
  }

  if (strcmp(type, P5) && strcmp(type, P2)) {
    printf("Error: Incorrect target PGM type, it should be either "P5" or "P2". Given type: %s\n", type);
    return;
  }

  PGM* pgm = readPGM(input);
  if (pgm == NULL) return;

  writePGMWithType(pgm, output, type);
  freePGM(pgm);

  printf("-> %s successfully rewrited in %s type to %s.\n", input, type, output);
}

/**
 * (7) Main
 */
#ifndef TEST_MODE
int main() {
  printf("-------------------------------------\n");
  printf("Welcome to PGM Lemonizer 2021\n");
  printf("Available commands are listed bellow:\n");
  help();
  
  while(true) {
    printf("$ ");
    char* line = scanLine();
    
    if (strcmp(line, "") != 0) {
      char* cmd = strtok(line, " ");

      if (strcmp(cmd, "compress") == 0) {
        menu_compress();
      } else if (strcmp(cmd, "decompress") == 0) {
        menu_decompress();
      } else if (strcmp(cmd, "replacecolor") == 0) {
        menu_replacecolor();
      } else if (strcmp(cmd, "setcolor") == 0) {
        menu_setcolor();
      } else if (strcmp(cmd, "histogram") == 0) {
        menu_histogram();
      } else if (strcmp(cmd, "convert") == 0) {
        menu_convert();
      } else if (strcmp(cmd, "help") == 0) {
        help();
      } else if (strcmp(cmd, "exit") == 0) {
        break;
      } else {
        printf("Error: Unknown command '%s'\n", cmd);
      }
    }

    free(line);
  }
  
  return 0;
}
#endif
