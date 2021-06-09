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
 * (5) Interface operations
 * (6) Main
 */

/**
 * (0) Program configuration definitions.
 */
#define LINESIZE 256
#define PGM_TYPE "P5"
#define CPGM_TYPE "CP5"
#define COMMENT_IDENTIFIER '#'
#define BLOCK_GROUP_SIZE 3
#define CPGM_BADGE ""
#define PGM_BADGE ""

/**
 * (1) Data types and structures.
 */
typedef uint16_t RunLength;
typedef uint8_t Pixel;
typedef uint8_t Block;
typedef struct PGM PGM;
typedef struct CPGM CPGM;
typedef struct RLENode RLENode;

struct PGM {
  Pixel* pixels;
  int maxValue;
  int width;
  int height;
};

struct CPGM {
  Block* blocks;
  int blockCount;
  int maxValue;
  int width;
  int height;
};

struct RLENode {
  RunLength runlength;
  Pixel pixel;
  RLENode* next;
  RLENode* prev;
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
PGM* createPGM(int maxValue, int width, int height) {
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

  pgm->maxValue = 255; //maxValue;
  pgm->width = width;
  pgm->height = height;

  return pgm;
}

void skipCommentsAndWhitespace(FILE* file) {
  int chr;
  
  // Ignore blank lines.
  while ((chr = fgetc(file)) != EOF && isspace(chr));

  // Go one byte back if not comment.
  if (chr != COMMENT_IDENTIFIER) {
    fseek(file, -1, SEEK_CUR);
    return;
  }

  // Skip line.
  char line[LINESIZE];
  fgets(line, sizeof(line), file);
    
  // Continue to skipping next comments.
  skipCommentsAndWhitespace(file);
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

  if (strcmp(pgmType, PGM_TYPE)) {
    printf("Error: Incorrect PGM type, it should be "PGM_TYPE". Given type: %s\n", pgmType);
    return NULL;
  }
  
  skipCommentsAndWhitespace(file);
  fscanf(file, "%d %d", &(width), &(height));
  skipCommentsAndWhitespace(file);
  fscanf(file, "%d", &(maxValue));
  fgetc(file); // Skip last whitespace before pixels.

  PGM* pgm = createPGM(maxValue, width, height);

  if (pgm == NULL) {
    printf("Error: Cannot read PGM due to memory allocation issues.");
    return NULL;
  }

  // Read pixel values into pgm.
  fread(pgm->pixels, sizeof(Pixel), pgm->width * pgm->height, file);
  fclose(file);

  return pgm;
}

void writePGM(PGM* pgm, char* filepath) {
  FILE* file = openFile(filepath, "wb");
  
  // Write header
  fprintf(file, 
    PGM_TYPE"\n"PGM_BADGE"\n%d %d\n%d\n",
    pgm->width,
    pgm->height,
    pgm->maxValue
  );
  
  // Write blocks.
  fwrite(pgm->pixels, sizeof(Pixel), pgm->width * pgm->height, file);
  
  fclose(file);
}

void destroyPGM(PGM* pgm) {
  free(pgm->pixels);
  free(pgm);
}

/**
 * (4) Compress / Decompress
 */
CPGM* createCPGM(int blockCount, int maxValue, int width, int height) {
  CPGM* cpgm = (CPGM*) malloc(sizeof(CPGM));

  if (cpgm == NULL) {
    printf("Error: Could not allocate memory for CPGM.");
    return NULL;
  }

  cpgm->blocks = (Block*) calloc(BLOCK_GROUP_SIZE * blockCount, sizeof(Block));

  if (cpgm->blocks == NULL) {
    printf("Error: Could not allocate memory for CPGM blocks.");
    return NULL;
  }

  cpgm->blockCount = blockCount;
  cpgm->maxValue = maxValue;
  cpgm->width = width;
  cpgm->height = height;

  return cpgm;
}

void insertCPGMBlockToBlocks(Block* blocks, RunLength runlength, Pixel pixel, int offset) {
  Block rlLSB = runlength & 0xFF;
  Block rlMSB = runlength >> 8;

  blocks[(offset * BLOCK_GROUP_SIZE)] = rlLSB;
  blocks[(offset * BLOCK_GROUP_SIZE) + 1] = rlMSB;
  blocks[(offset * BLOCK_GROUP_SIZE) + 2] = pixel;
}

CPGM* compressPGM(PGM* pgm) {
  int pixelCount = pgm->width * pgm->height;
  int maxBlockCapacity = 16;
  int blockGroupCount = 0;
  
  Block* blocks = (Block *) calloc(BLOCK_GROUP_SIZE * maxBlockCapacity, sizeof(Block));
  Pixel pixel = pgm->pixels[0];
  RunLength runlength = 0;

  int i;
  for (i = 0; i < pixelCount; i++) {
    // Resize if needed.
    if (blockGroupCount == maxBlockCapacity) {
      maxBlockCapacity *= 2;
      blocks = (Block *) realloc(blocks, BLOCK_GROUP_SIZE * maxBlockCapacity * sizeof(Block));
    }

    Pixel nextPixel = pgm->pixels[i];

    if (nextPixel != pixel) {
      insertCPGMBlockToBlocks(blocks, runlength, pixel, blockGroupCount);

      runlength = 0;
      pixel = nextPixel;
      blockGroupCount++;
    }

    runlength++;
  }

  if (runlength > 0) {
    insertCPGMBlockToBlocks(blocks, runlength, pixel, blockGroupCount);
    blockGroupCount++;
  }

  CPGM* cpgm = createCPGM(BLOCK_GROUP_SIZE * blockGroupCount, pgm->maxValue, pgm->width, pgm->height);

  if (cpgm == NULL) {
    printf("Error: Could not compress PGM due to memory issues.");
    return NULL;
  }

  cpgm->blocks = blocks;

  return cpgm;
}

bool validateCPGM(CPGM* cpgm) {
  // a
  // b
  // c
  return true;
}

PGM* decompressCPGM(CPGM* cpgm) {
  if (!validateCPGM(cpgm)) {
    printf("Error: Given CPGM is not valid state.\n");
    return NULL;
  }
  
  PGM* pgm = createPGM(cpgm->maxValue, cpgm->width, cpgm->height);

  int i = 0;
  int j;
  for (j = 0; j < cpgm->blockCount; j += 3) {
    Block rlLSB = cpgm->blocks[j];
    Block rlMSB = cpgm->blocks[j + 1];
    Pixel pixel = cpgm->blocks[j + 2];
    RunLength runlength = (rlMSB << 8) | rlLSB;

    memset(pgm->pixels + i, pixel, runlength);
    i += runlength;
  }

  return pgm;
}

CPGM* readCPGM(char* filepath) {
  FILE* file = openFile(filepath, "rb");

  if (file == NULL) {
    printf("Error: Cannot find CPGM file in given path: %s\n", filepath);
    return NULL;
  }

  char cpgmType[LINESIZE];
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
  fscanf(file, "%d %d %d %d", &(width), &(height), &(maxValue), &(blockCount));
  fgetc(file); // Skip last whitespace before pixels.

  CPGM* cpgm = createCPGM(blockCount, maxValue, width, height);

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
    CPGM_TYPE"\n"CPGM_BADGE"\n%d %d %d %d\n",
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
  free(cpgm->blocks);
  free(cpgm);
}

/**
 * (5) Interface operations
 */

/**
 * (6) Main
 */
int main() {
  PGM* pgm_read = readPGM("pgms/gman.pgm");
  CPGM* cpgm_original = compressPGM(pgm_read);

  writeCPGM(cpgm_original, "tmp/tzest.cpgm");
  CPGM* cpgm_read = readCPGM("tmp/tzest.cpgm");

  PGM* pgm_dcomp = decompressCPGM(cpgm_original);
  PGM* pgm_dcomp_read = decompressCPGM(cpgm_read);

  writePGM(pgm_read, "tmp/test_read.pgm");
  writePGM(pgm_dcomp, "tmp/test_dcomp.pgm");
  writePGM(pgm_dcomp_read, "tmp/test_dcomp_read.pgm");

  return 0;
}
