#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

void skip(FILE* fp){
  int ch;
  char line[256];
  while ((ch = fgetc(fp)) != EOF && isspace(ch));
  if (ch == '#') {
    fgets(line, sizeof(line), fp);
    skip(fp);
  } else {
    fseek(fp, -1, SEEK_CUR);
  }
}

int comparePGMFiles(char* fname_a, char* fname_b) {
  FILE* file_a = fopen(fname_a, "rb");
  FILE* file_b = fopen(fname_b, "rb");
  
  if (file_a == NULL) {
    printf("FU: File %s does not exist.\n", fname_a);
    return 1;
  }

  if (file_b == NULL) {
    printf("FU: File %s does not exist.\n", fname_b);
    return 1;
  }

  char ptype_a[5];
  char ptype_b[5];
  int width_a;
  int height_a;
  int width_b;
  int height_b;
  int max_a;
  int max_b;

  skip(file_a);
  skip(file_b);

  fscanf(file_a, "%s", ptype_a);
  fscanf(file_b, "%s", ptype_b);

  if (strcmp(ptype_a, ptype_b)) {
    printf("FU: Type mismatch %s =/= %s.\n", ptype_a, ptype_b);
    return 1;
  }

  skip(file_a);
  skip(file_b);

  fscanf(file_a, "%d %d", &(width_a), &(height_a));
  fscanf(file_b, "%d %d", &(width_b), &(height_b));

  if (width_a != width_b) {
    printf("FU: Width mismatch %d =/= %d.\n", width_a, width_b);
    return 1;
  }

  if (height_a != height_b) {
    printf("FU: Height mismatch %d =/= %d.\n", height_a, height_b);
    return 1;
  }

  skip(file_a);
  skip(file_b);

  fscanf(file_a, "%d", &(max_a));
  fscanf(file_b, "%d", &(max_b));
  
  if (max_a != max_b) {
    printf("FU: Max value mismatch %d =/= %d.\n", max_a, max_b);
    return 1;
  }

  fgetc(file_a);
  fgetc(file_b);

  int pixelCount = width_a * height_a;

  uint8_t* pixels_a = (uint8_t*) calloc(pixelCount, sizeof(uint8_t));
  uint8_t* pixels_b = (uint8_t*) calloc(pixelCount, sizeof(uint8_t));

  if (strcmp(ptype_a, "P5") == 0) {
    fread(pixels_a, sizeof(uint8_t), pixelCount, file_a);
    fread(pixels_b, sizeof(uint8_t), pixelCount, file_b);
  } else {
    int i;
    for (i = 0; i < pixelCount; i++) {
      fscanf(file_a, "%hhu ", &(pixels_a[i]));
      fscanf(file_b, "%hhu ", &(pixels_b[i]));
    }
  }

  if (memcmp(pixels_a, pixels_b, pixelCount) != 0) {
    printf("FU: Data mismatch.\n");
    return 1;
  }
  
  printf("Aferin! PGM files are identical!\n");

  fclose(file_a);
  fclose(file_b);

  return 0;
}

int main(int argc, char **argv) {
  char* fname_a = argv[1];
  char* fname_b = argv[2];

  return comparePGMFiles(fname_a, fname_b);
}
