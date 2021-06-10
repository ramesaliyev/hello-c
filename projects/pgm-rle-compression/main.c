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
#define BLOCK_GROUP_SIZE 3

/**
 * (1) Data types and structures.
 */
typedef uint16_t RunLength;
typedef uint8_t Pixel;
typedef uint8_t Block;
typedef struct PGM PGM;
typedef struct CPGM CPGM;
typedef struct BlockGroup BlockGroup;

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

struct BlockGroup {
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

void readP5Pixels(PGM* pgm, FILE* file) {
  
}

void readP2Pixels(PGM* pgm, FILE* file) {
  
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

  if (strcmp(pgmType, P5)) {
    printf("Error: Incorrect PGM type, it should be "P5". Given type: %s\n", pgmType);
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

  // Read pixel values into pgm.
  fread(pgm->pixels, sizeof(Pixel), pgm->width * pgm->height, file);
  fclose(file);

  return pgm;
}

void writeP5Pixels(PGM* pgm, FILE* file) {

}

void writeP2Pixels(PGM* pgm, FILE* file) {
  
}

void writePGM(PGM* pgm, char* filepath) {
  FILE* file = openFile(filepath, "wb");
  
  // Write header
  fprintf(file, 
    "%s\n%d %d\n%d\n",
    pgm->type,
    pgm->width,
    pgm->height,
    pgm->maxValue
  );
  
  // Write blocks.
  fwrite(pgm->pixels, sizeof(Pixel), pgm->width * pgm->height, file);
  
  fclose(file);
}

void destroyPGM(PGM* pgm) {
  free(pgm->type);
  free(pgm->pixels);
  free(pgm);
}

/**
 * (4) Compress / Decompress
 */
BlockGroup* createBlockGroup() {
  return (BlockGroup*) malloc(sizeof(BlockGroup));
}

void updateBlockGroup(BlockGroup* blockGroup, CPGM* cpgm, int offset) {
  Block rlLSB = cpgm->blocks[offset];
  Block rlMSB = cpgm->blocks[offset + 1];
  blockGroup->pixel = cpgm->blocks[offset + 2];
  blockGroup->runlength = (rlMSB << 8) | rlLSB;
}

void freeBlockGroup(BlockGroup* blockGroup) {
  free(blockGroup);
}

void insertCPGMBlockToBlocks(Block* blocks, RunLength runlength, Pixel pixel, int offset) {
  Block rlLSB = runlength & 0xFF;
  Block rlMSB = runlength >> 8;

  blocks[(offset * BLOCK_GROUP_SIZE)] = rlLSB;
  blocks[(offset * BLOCK_GROUP_SIZE) + 1] = rlMSB;
  blocks[(offset * BLOCK_GROUP_SIZE) + 2] = pixel;
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

  cpgm->blocks = (Block*) calloc(BLOCK_GROUP_SIZE * blockCount, sizeof(Block));

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

  CPGM* cpgm = createCPGM(BLOCK_GROUP_SIZE * blockGroupCount, pgm->type, pgm->maxValue, pgm->width, pgm->height);

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
  
  BlockGroup* blockGroup = createBlockGroup();
  PGM* pgm = createPGM(cpgm->pgmType, cpgm->maxValue, cpgm->width, cpgm->height);

  int i = 0;
  int j;
  for (j = 0; j < cpgm->blockCount; j += 3) {
    updateBlockGroup(blockGroup, cpgm, j);

    memset(pgm->pixels + i, blockGroup->pixel, blockGroup->runlength);
    i += blockGroup->runlength;
  }

  freeBlockGroup(blockGroup);

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
  BlockGroup* blockGroup = createBlockGroup();

  int j;
  for (j = 0; j < cpgm->blockCount; j += 3) {
    updateBlockGroup(blockGroup, cpgm, j);
    colors[blockGroup->pixel] += blockGroup->runlength;
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

  freeBlockGroup(blockGroup);
}

/**
 * (6) Interface operations
 */

/**
 * (7) Main
 */
int main() {
  char* input = "pgms/gman.pgm";

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
