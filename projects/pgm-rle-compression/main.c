#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

/**
 * Code structure:
 * Search for (x) to go section.
 * 
 * (0) Program configuration definitions.
 * (1) Data types and structures.
 * (2) Common generic utilities.
 * (3) PGM related functions.
 * (4) Compress / Decompress
 * (5) Operations on compressed file.
 * (6) Interface operations
 * (7) Main
 */

/**
 * (0) Program configuration definitions.
 */
#define LINESIZE 256
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
  int maxValue;
  int width;
  int height;
};

struct Entry {
  RunLength runlength;
  Pixel pixel;
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

FILE* openFile(char* filename, char* as) {
  FILE* file = fopen(filename, as);
  if (file == NULL) {
    printf("Error: Could not open file; name=%s, mode=%s", filename, as);
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
    printf("Error: Cannot read PGM due to memory allocation issues.");
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
      pgm->pixels[i] = (Pixel) color;
      printf("%d %d\n", color, pgm->pixels[i]);
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
    int i, color;
    for (i = 0; i < pixelCount; i++) {
      fprintf(file, "%d%c", pgm->pixels[i], ((i+1) % pgm->width) ? ' ' : '\n');
    }
  }
  
  fclose(file);
}

void writePGM(PGM* pgm, char* filepath) {
  writePGMWithType(pgm, filepath, pgm->type);
}

void destroyPGM(PGM* pgm) {
  free(pgm->type);
  free(pgm->pixels);
  free(pgm);
}

/**
 * (4) Compress / Decompress
 */
Entry* createEntry() {
  return (Entry*) malloc(sizeof(Entry));
}

void updateEntry(Entry* entry, CPGM* cpgm, int offset) {
  Block rlLSB = cpgm->blocks[offset];
  Block rlMSB = cpgm->blocks[offset + 1];
  entry->pixel = cpgm->blocks[offset + 2];
  entry->runlength = (rlMSB << 8) | rlLSB;
}

void freeEntry(Entry* entry) {
  free(entry);
}

void insertBlock(Block* blocks, RunLength runlength, Pixel pixel, int offset) {
  Block rlLSB = runlength & 0xFF;
  Block rlMSB = runlength >> 8;

  blocks[(offset * BLOCK_PER_ENTRY)] = rlLSB;
  blocks[(offset * BLOCK_PER_ENTRY) + 1] = rlMSB;
  blocks[(offset * BLOCK_PER_ENTRY) + 2] = pixel;
}

bool validateCPGM(CPGM* cpgm) {
  // a
  // b
  // c
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
  
  Entry* entry = createEntry();
  PGM* pgm = createPGM(cpgm->pgmType, cpgm->maxValue, cpgm->width, cpgm->height);

  int i = 0;
  int j;
  for (j = 0; j < cpgm->blockCount; j += 3) {
    updateEntry(entry, cpgm, j);

    memset(pgm->pixels + i, entry->pixel, entry->runlength);
    i += entry->runlength;
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

void destroyCPGM(CPGM* cpgm) {
  free(cpgm->pgmType);
  free(cpgm->blocks);
  free(cpgm);
}

/**
 * (5) Operations on compressed file.
 */
void printHistogram(CPGM* cpgm) {
  RunLength colors[MAX_COLOR + 1] = {0};
  Entry* entry = createEntry();

  int j;
  for (j = 0; j < cpgm->blockCount; j += 3) {
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

/**
 * (7) Main
 */
int main() {
  char* input = "pgms/test.pgm";

  PGM* pgm_read = readPGM(input);
  CPGM* cpgm_original = compressPGM(pgm_read);

  writeCPGM(cpgm_original, "tmp/tzest.cpgm");
  CPGM* cpgm_read = readCPGM("tmp/tzest.cpgm");

  PGM* pgm_dcomp = decompressCPGM(cpgm_original);
  PGM* pgm_dcomp_read = decompressCPGM(cpgm_read);

  writePGM(pgm_read, "tmp/test_read.pgm");
  writePGM(pgm_dcomp, "tmp/test_dcomp.pgm");
  writePGM(pgm_dcomp_read, "tmp/test_dcomp_read.pgm");

  // printHistogram(cpgm_original);

  return 0;
}
