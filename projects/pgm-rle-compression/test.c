#define TEST_MODE
#include "main.c"
#include "tools/compare_pgms.c"

int testCount = 0;
void assert(bool passed, char* error) {
  testCount++;
  if (!passed) {
    printf("%d asserts are passed, error on:\n", testCount);
    printf("%s\n", error);
    exit(1);
  }
}

void testP2Read() {
  PGM* pgm = readPGM("pgms/test/p2.read.1.pgm");
  assert(pgm->width == 640, "Should read width correctly.");
  assert(pgm->height == 480, "Should read height correctly.");
  assert(pgm->maxValue == 255, "Should read maxValue correctly.");
  freePGM(pgm);
}

void testP2ReadWriteRead() {
  PGM* pgm = readPGM("pgms/test/p2.read.1.pgm");
  writePGM(pgm, "tmp/test/testP2ReadWriteRead.pgm");
  PGM* pgm_read = readPGM("pgms/test/p2.read.1.pgm");
  assert(pgm->width == pgm_read->width, "Should read width correctly.");
  assert(pgm->height == pgm_read->height, "Should read height correctly.");
  assert(pgm->maxValue == pgm_read->maxValue, "Should read maxValue correctly.");
  freePGM(pgm);
}

void testP2Compress() {
  PGM* pgm = readPGM("pgms/test/p2.compress.1.pgm");
  CPGM* cpgm = compressPGM(pgm);
  assert(cpgm->width == pgm->width, "Should transfer width correctly.");
  assert(cpgm->height == pgm->height, "Should transfer height correctly.");
  assert(cpgm->maxValue == pgm->maxValue, "Should transfer maxValue correctly.");
  assert(cpgm->blockCount == 3, "Block count should be 3.");
  assert(cpgm->entryCount == 1, "Entry count should be 1.");
  freePGM(pgm);
  freeCPGM(cpgm);
}

void testP2Compress2() {
  PGM* pgm = readPGM("pgms/test/p2.compress.2.pgm");
  CPGM* cpgm = compressPGM(pgm);
  assert(cpgm->width == pgm->width, "Should transfer width correctly.");
  assert(cpgm->height == pgm->height, "Should transfer height correctly.");
  assert(cpgm->maxValue == pgm->maxValue, "Should transfer maxValue correctly.");
  assert(cpgm->blockCount == 45, "Block count should be 45.");
  assert(cpgm->entryCount == 15, "Entry count should be 15.");
  freePGM(pgm);
  freeCPGM(cpgm);
}

void testP2CPGMWriteReadDecompress() {
  PGM* pgm = readPGM("pgms/test/p2.cpgm.write.1.pgm");
  CPGM* cpgm = compressPGM(pgm);
  writeCPGM(cpgm, "tmp/test/testP2CPGMWriteReadDecompress.cpgm");
  writePGM(pgm, "tmp/test/testP2CPGMWriteReadDecompress.pgm");

  CPGM* cpgm_read = readCPGM("tmp/test/testP2CPGMWriteReadDecompress.cpgm");
  PGM* pgm_read = readPGM("tmp/test/testP2CPGMWriteReadDecompress.pgm");
  CPGM* cpgm_read_comp = compressPGM(pgm_read);
  PGM* pgm_dcomp = decompressCPGM(cpgm);
  PGM* pgm_dcomp_read = decompressCPGM(cpgm_read);

  assert(cpgm->width == pgm->width, "Should transfer width correctly.");
  assert(cpgm->height == pgm->height, "Should transfer height correctly.");
  assert(cpgm->maxValue == pgm->maxValue, "Should transfer maxValue correctly.");
  assert(cpgm_read_comp->width == cpgm->width, "Should read width correctly.");
  assert(cpgm_read_comp->height == cpgm->height, "Should read height correctly.");
  assert(cpgm_read_comp->maxValue == cpgm->maxValue, "Should read maxValue correctly.");
  assert(cpgm_read_comp->blockCount == cpgm->blockCount, "Should read blockCount correctly.");
  assert(cpgm_read_comp->entryCount == cpgm->entryCount, "Should read entryCount correctly.");
  assert(cpgm_read_comp->width == cpgm->width, "Should read width correctly.");
  assert(cpgm_read_comp->height == cpgm->height, "Should read height correctly.");
  assert(cpgm_read_comp->maxValue == cpgm->maxValue, "Should read maxValue correctly.");
  assert(cpgm_read_comp->blockCount == cpgm->blockCount, "Should read blockCount correctly.");
  assert(cpgm_read_comp->entryCount == cpgm->entryCount, "Should read entryCount correctly.");
  assert(pgm->width == pgm_dcomp->width, "Should decompress width correctly.");
  assert(pgm->height == pgm_dcomp->height, "Should decompress height correctly.");
  assert(pgm->maxValue == pgm_dcomp->maxValue, "Should decompress maxValue correctly.");
  assert(pgm->width == pgm_dcomp_read->width, "Should decompress width correctly.");
  assert(pgm->height == pgm_dcomp_read->height, "Should decompress height correctly.");
  assert(pgm->maxValue == pgm_dcomp_read->maxValue, "Should decompress maxValue correctly.");

  freePGM(pgm);
  freePGM(pgm_dcomp);
  freePGM(pgm_dcomp_read);
  freeCPGM(cpgm);
  freeCPGM(cpgm_read);
}

void testP2ReplaceColorSingle() {
  PGM* pgm = readPGM("pgms/test/p2.replacecolor.1.pgm");
  CPGM* cpgm = compressPGM(pgm);

  replaceColor(cpgm, 255, 199);
  assert(cpgm->blockCount == 3, "Should have only one color.");
  assert(cpgm->entryCount == 1, "Should have only one color.");
  assert(cpgm->blocks[2] == 199, "Should replace single color.");

  freePGM(pgm);
  freeCPGM(cpgm);
}

void testP2ReplaceColorOccurences() {
  PGM* pgm = readPGM("pgms/test/p2.replacecolor.2.pgm");
  CPGM* cpgm = compressPGM(pgm);

  replaceColor(cpgm, 100, 199);
  assert(cpgm->blockCount == 27, "Should have correct block count.");
  assert(cpgm->entryCount == 9, "Should have correct entry count.");
  assert(cpgm->blocks[1*3+2] == 199, "Should replace multiple occurences color 1.");
  assert(cpgm->blocks[3*3+2] == 199, "Should replace multiple occurences color 2.");
  assert(cpgm->blocks[5*3+2] == 199, "Should replace multiple occurences color 3.");
  assert(cpgm->blocks[7*3+2] == 199, "Should replace multiple occurences color 4.");

  freePGM(pgm);
  freeCPGM(cpgm);
}

void testP2ReplaceColorFirstLast() {
  PGM* pgm = readPGM("pgms/test/p2.replacecolor.3.pgm");
  CPGM* cpgm = compressPGM(pgm);

  replaceColor(cpgm, 100, 177);
  assert(cpgm->blockCount == 9, "Should have correct block count.");
  assert(cpgm->entryCount == 3, "Should have correct entry count.");
  assert(cpgm->blocks[2] == 177, "Should replace first color.");
  assert(cpgm->blocks[cpgm->blockCount-1] == 177, "Should replace last color.");

  freePGM(pgm);
  freeCPGM(cpgm);
}

void testP2ReplaceColorReduceToOne() {
  PGM* pgm = readPGM("pgms/test/p2.replacecolor.4.pgm");
  CPGM* cpgm = compressPGM(pgm);

  replaceColor(cpgm, 100, 202);
  replaceColor(cpgm, 101, 202);
  replaceColor(cpgm, 102, 202);
  replaceColor(cpgm, 103, 202); // Not exist
  replaceColor(cpgm, 155, 202);
  replaceColor(cpgm, 189, 202);
  replaceColor(cpgm, 190, 202); // Not exist
  replaceColor(cpgm, 255, 202);

  assert(cpgm->blockCount == 3, "Should have correct block count.");
  assert(cpgm->entryCount == 1, "Should have correct entry count.");
  assert(cpgm->blocks[2] == 202, "Should replace all colors.");


  freePGM(pgm);
  freeCPGM(cpgm);
}

void testP2ReplaceColorStandingLast() {
  PGM* pgm = readPGM("pgms/test/p2.replacecolor.5.pgm");
  CPGM* cpgm = compressPGM(pgm);

  replaceColor(cpgm, 255, 177);
  assert(cpgm->blockCount == 6, "Should have correct block count.");
  assert(cpgm->entryCount == 2, "Should have correct entry count.");
  assert(cpgm->blocks[2] == 177, "Should replace first color.");
  assert(cpgm->blocks[cpgm->blockCount-1] == 199, "Should not replace last color.");

  freePGM(pgm);
  freeCPGM(cpgm);
}

void testP2SetColorSingular() {
  PGM* pgm = readPGM("pgms/test/p2.setcolor.1.pgm");
  CPGM* cpgm = compressPGM(pgm);

  assert(cpgm->blockCount == 3, "Should have correct block count.");
  assert(cpgm->entryCount == 1, "Should have correct entry count.");

  setColor(cpgm, 0, 0, 189);
  assert(cpgm->blockCount == 6, "Should change to correct block count.");
  assert(cpgm->entryCount == 2, "Should change to correct entry count.");
  assert(cpgm->blocks[2] == 189, "Should replace first color.");

  setColor(cpgm, 5, 5, 199);
  assert(cpgm->blockCount == 9, "Should change to correct block count.");
  assert(cpgm->entryCount == 3, "Should change to correct entry count.");
  assert(cpgm->blocks[8] == 199, "Should replace first color.");

  assert(cpgm->blocks[3] == 34, "Should keep other color."); // LSB => run length
  assert(cpgm->blocks[5] == 255, "Should keep other color.");

  freePGM(pgm);
  freeCPGM(cpgm);
}

void testP2SetColorMergeLeftToNextFirst() {
  PGM* pgm = readPGM("pgms/test/p2.setcolor.2.pgm");
  CPGM* cpgm = compressPGM(pgm);

  assert(cpgm->entryCount == 3, "Should have correct entry count.");
  setColor(cpgm, 0, 0, 177);
  assert(cpgm->entryCount == 2, "Should change to correct entry count.");
  assert(cpgm->blocks[2] == 177, "Should replace first color.");
  assert(cpgm->blocks[0] == 2, "Should change to correct runlength.");
  assert(cpgm->blocks[5] == 255, "Should keep other color.");
  assert(cpgm->blocks[3] == 34, "Should change to correct runlength.");

  freePGM(pgm);
  freeCPGM(cpgm);
}

void testP2SetColorMergeLeftToNextMiddle() {
  PGM* pgm = readPGM("pgms/test/p2.setcolor.3.pgm");
  CPGM* cpgm = compressPGM(pgm);

  assert(cpgm->entryCount == 4, "Should have correct entry count.");
  setColor(cpgm, 0, 2, 177);
  assert(cpgm->entryCount == 3, "Should change to correct entry count.");
  assert(cpgm->blocks[5] == 177, "Should replace first color.");
  assert(cpgm->blocks[3] == 2, "Should change to correct runlength.");
  assert(cpgm->blocks[2] == 255, "Should keep other color.");
  assert(cpgm->blocks[0] == 2, "Should keep correct runlength.");
  assert(cpgm->blocks[8] == 255, "Should keep other color.");
  assert(cpgm->blocks[6] == 32, "Should keep correct runlength.");

  freePGM(pgm);
  freeCPGM(cpgm);
}

void testP2SetColorMergeLeftToNextLast() {
  PGM* pgm = readPGM("pgms/test/p2.setcolor.4.pgm");
  CPGM* cpgm = compressPGM(pgm);

  assert(cpgm->entryCount == 3, "Should have correct entry count.");
  setColor(cpgm, 5, 4, 177);
  assert(cpgm->entryCount == 2, "Should change to correct entry count.");
  assert(cpgm->blocks[2] == 255, "Should keep first color.");
  assert(cpgm->blocks[0] == 34, "Should keep the correct runlength.");
  assert(cpgm->blocks[5] == 177, "Should replace to last color.");
  assert(cpgm->blocks[3] == 2, "Should change to correct runlength.");

  freePGM(pgm);
  freeCPGM(cpgm);
}

void testP2SetColorMergeRightToPrevFirst() {
  PGM* pgm = readPGM("pgms/test/p2.setcolor.2.pgm");
  CPGM* cpgm = compressPGM(pgm);

  assert(cpgm->entryCount == 3, "Should have correct entry count.");
  setColor(cpgm, 0, 1, 100);
  assert(cpgm->entryCount == 2, "Should change to correct entry count.");
  assert(cpgm->blocks[2] == 100, "Should replace first color.");
  assert(cpgm->blocks[0] == 2, "Should change to correct runlength.");
  assert(cpgm->blocks[5] == 255, "Should keep other color.");
  assert(cpgm->blocks[3] == 34, "Should change to correct runlength.");

  freePGM(pgm);
  freeCPGM(cpgm);
}

void testP2SetColorMergeRightToPrevMiddle() {
  PGM* pgm = readPGM("pgms/test/p2.setcolor.3.pgm");
  CPGM* cpgm = compressPGM(pgm);

  assert(cpgm->entryCount == 4, "Should have correct entry count.");
  setColor(cpgm, 0, 3, 100);
  assert(cpgm->entryCount == 3, "Should change to correct entry count.");
  assert(cpgm->blocks[5] == 100, "Should replace first color.");
  assert(cpgm->blocks[3] == 2, "Should change to correct runlength.");
  assert(cpgm->blocks[2] == 255, "Should keep other color.");
  assert(cpgm->blocks[0] == 2, "Should keep correct runlength.");
  assert(cpgm->blocks[8] == 255, "Should keep other color.");
  assert(cpgm->blocks[6] == 32, "Should keep correct runlength.");

  freePGM(pgm);
  freeCPGM(cpgm);
}

void testP2SetColorMergeRightToPrevLast() {
  PGM* pgm = readPGM("pgms/test/p2.setcolor.4.pgm");
  CPGM* cpgm = compressPGM(pgm);

  assert(cpgm->entryCount == 3, "Should have correct entry count.");
  setColor(cpgm, 5, 5, 100);
  assert(cpgm->entryCount == 2, "Should change to correct entry count.");
  assert(cpgm->blocks[2] == 255, "Should keep first color.");
  assert(cpgm->blocks[0] == 34, "Should keep the correct runlength.");
  assert(cpgm->blocks[5] == 100, "Should replace to last color.");
  assert(cpgm->blocks[3] == 2, "Should change to correct runlength.");

  freePGM(pgm);
  freeCPGM(cpgm);
}

void testP2SetColorMergeMiddleWithSurrounding() {
  PGM* pgm = readPGM("pgms/test/p2.setcolor.5.pgm");
  CPGM* cpgm = compressPGM(pgm);

  assert(cpgm->entryCount == 11, "Should have correct entry count.");

  // First
  freeCPGM(cpgm);
  cpgm = compressPGM(pgm);
  setColor(cpgm, 0, 1, 100);
  assert(cpgm->entryCount == 9, "Should change to correct entry count.");
  assert(cpgm->blocks[2] == 100, "Should change color.");
  assert(cpgm->blocks[0] == 3, "Should change runlength.");
  assert(cpgm->blocks[5] == 255, "Should keep other color.");

  // Middle
  freeCPGM(cpgm);
  cpgm = compressPGM(pgm);
  setColor(cpgm, 2, 2, 100);
  assert(cpgm->entryCount == 9, "Should change to correct entry count.");
  assert(cpgm->blocks[14] == 100, "Should change color.");
  assert(cpgm->blocks[12] == 3, "Should change runlength.");
  assert(cpgm->blocks[11] == 255, "Should keep other color.");

  // Last
  freeCPGM(cpgm);
  cpgm = compressPGM(pgm);
  setColor(cpgm, 5, 4, 100);
  assert(cpgm->entryCount == 9, "Should change to correct entry count.");
  assert(cpgm->blocks[26] == 100, "Should change color.");
  assert(cpgm->blocks[24] == 3, "Should change runlength.");
  assert(cpgm->blocks[23] == 255, "Should keep other color.");

  // All in once.
  freeCPGM(cpgm);
  cpgm = compressPGM(pgm);
  setColor(cpgm, 0, 1, 100);
  setColor(cpgm, 2, 2, 100);
  setColor(cpgm, 5, 4, 100);
  assert(cpgm->entryCount == 5, "Should change to correct entry count.");
  assert(cpgm->blocks[0] == 3, "Should change runlength.");
  assert(cpgm->blocks[2] == 100, "Should change color.");
  assert(cpgm->blocks[5] == 255, "Should keep color.");
  assert(cpgm->blocks[6] == 3, "Should change runlength.");
  assert(cpgm->blocks[8] == 100, "Should change color.");
  assert(cpgm->blocks[11] == 255, "Should keep color.");
  assert(cpgm->blocks[12] == 3, "Should change runlength.");
  assert(cpgm->blocks[14] == 100, "Should change color.");

  freePGM(pgm);
  freeCPGM(cpgm);
}

void testP2SetColorSplitEntry() {
  PGM* pgm = readPGM("pgms/test/p2.setcolor.6.pgm");
  CPGM* cpgm = compressPGM(pgm);

  assert(cpgm->entryCount == 5, "Should have correct entry count.");

  // On left edge
  setColor(cpgm, 0, 0, 199);
  assert(cpgm->entryCount == 6, "Should change to correct entry count.");
  assert(cpgm->blocks[0] == 1, "Should change runlength.");
  assert(cpgm->blocks[2] == 199, "Should change color.");
  assert(cpgm->blocks[3] == 2, "Should change runlength.");
  assert(cpgm->blocks[5] == 100, "Should keep color.");

  // On center
  freeCPGM(cpgm);
  cpgm = compressPGM(pgm);
  setColor(cpgm, 2, 3, 199);
  assert(cpgm->entryCount == 7, "Should change to correct entry count.");
  assert(cpgm->blocks[6] == 3, "Should change runlength.");
  assert(cpgm->blocks[8] == 100, "Should keep color.");
  assert(cpgm->blocks[9] == 1, "Should change runlength.");
  assert(cpgm->blocks[11] == 199, "Should change color.");
  assert(cpgm->blocks[12] == 2, "Should change runlength.");
  assert(cpgm->blocks[14] == 100, "Should keep color.");

  // On right edge
  freeCPGM(cpgm);
  cpgm = compressPGM(pgm);
  setColor(cpgm, 5, 5, 199);
  assert(cpgm->entryCount == 6, "Should change to correct entry count.");
  assert(cpgm->blocks[12] == 2, "Should change runlength.");
  assert(cpgm->blocks[14] == 100, "Should keep color.");
  assert(cpgm->blocks[15] == 1, "Should change runlength.");
  assert(cpgm->blocks[17] == 199, "Should change color.");

  freePGM(pgm);
  freeCPGM(cpgm);
}

void testP2SetColorSplitMerge() {
  PGM* pgm = readPGM("pgms/test/p2.setcolor.7.pgm");
  CPGM* cpgm = compressPGM(pgm);

  assert(cpgm->entryCount == 3, "Should have correct entry count.");
  assert(cpgm->blocks[0] == 3, "Should have correct runlengths 1.");
  assert(cpgm->blocks[3] == 30, "Should have correct runlengths 2.");
  assert(cpgm->blocks[6] == 3, "Should have correct runlengths 3.");
  assert(cpgm->blocks[2] == 100, "Should have correct colors 1.");
  assert(cpgm->blocks[5] == 255, "Should have correct colors 2.");
  assert(cpgm->blocks[8] == 100, "Should have correct colors 3.");

  // Merge to left
  setColor(cpgm, 5, 3, 255);
  assert(cpgm->entryCount == 3, "Should keep entry count.");
  assert(cpgm->blocks[0] == 3, "Should change to correct runlengths 1.");
  assert(cpgm->blocks[3] == 31, "Should change to correct runlengths 2.");
  assert(cpgm->blocks[6] == 2, "Should change to correct runlengths 3.");
  assert(cpgm->blocks[2] == 100, "Should have correct colors 1.");
  assert(cpgm->blocks[5] == 255, "Should have correct colors 2.");
  assert(cpgm->blocks[8] == 100, "Should have correct colors 3.");

  // Merge to right
  freeCPGM(cpgm);
  cpgm = compressPGM(pgm);
  setColor(cpgm, 0, 2, 255);
  assert(cpgm->entryCount == 3, "Should keep entry count.");
  assert(cpgm->blocks[0] == 2, "Should change to correct runlengths 1.");
  assert(cpgm->blocks[3] == 31, "Should change to correct runlengths 2.");
  assert(cpgm->blocks[6] == 3, "Should change to correct runlengths 3.");
  assert(cpgm->blocks[2] == 100, "Should have correct colors 1.");
  assert(cpgm->blocks[5] == 255, "Should have correct colors 2.");
  assert(cpgm->blocks[8] == 100, "Should have correct colors 3.");
  
  freePGM(pgm);
  freeCPGM(cpgm);
}

void testP2SomeExtraCases1() {
  PGM* pgm = readPGM("pgms/test/extra/p2.test.1.in.pgm");
  CPGM* cpgm = compressPGM(pgm);
  setColor(cpgm, 0, 3, 100);
  replaceColor(cpgm, 50, 100);
  replaceColor(cpgm, 100, 50);
  PGM* pgm2 = decompressCPGM(cpgm);
  writePGM(pgm2, "tmp/test/p2.test.1.out.pgm");
  assert(cpgm->entryCount == 3, "Should have correct entry count.");
  assert(
    comparePGMFiles("tmp/test/p2.test.1.out.pgm", "pgms/test/extra/p2.test.1.out.pgm") == 0,
    "Output PGM should be correct."
  );
  freeCPGM(cpgm);
  freePGM(pgm);
  freePGM(pgm2);
}

void testP2SomeExtraCases2() {
  PGM* pgm = readPGM("pgms/test/extra/p2.test.2.in.pgm");
  CPGM* cpgm = compressPGM(pgm);
  PGM* pgm2 = decompressCPGM(cpgm);
  writePGM(pgm2, "tmp/test/p2.test.2.out.pgm");
  assert(
    comparePGMFiles("tmp/test/p2.test.2.out.pgm", "pgms/test/extra/p2.test.2.out.pgm") == 0,
    "Output PGM should be correct."
  );
  freeCPGM(cpgm);
  freePGM(pgm);
  freePGM(pgm2);
}

void testP2SomeExtraCases3() {
  PGM* pgm = readPGM("pgms/test/extra/p2.test.3.in.pgm");
  CPGM* cpgm = compressPGM(pgm);
  setColor(cpgm, 0, 2, 10);
  PGM* pgm2 = decompressCPGM(cpgm);
  writePGM(pgm2, "tmp/test/p2.test.3.out.pgm");
  assert(
    comparePGMFiles("tmp/test/p2.test.3.out.pgm", "pgms/test/extra/p2.test.3.out.pgm") == 0,
    "Output PGM should be correct."
  );
  freeCPGM(cpgm);
  freePGM(pgm);
  freePGM(pgm2);
}

void testP2SomeExtraCases4() {
  PGM* pgm = readPGM("pgms/test/extra/p2.test.4.in.pgm");
  CPGM* cpgm = compressPGM(pgm);
  setColor(cpgm, 1, 0, 10);
  setColor(cpgm, 0, 0, 0);
  setColor(cpgm, 2, 3, 0);
  replaceColor(cpgm, 10, 0);
  PGM* pgm2 = decompressCPGM(cpgm);
  writePGM(pgm2, "tmp/test/p2.test.4.out.pgm");
  assert(
    comparePGMFiles("tmp/test/p2.test.4.out.pgm", "pgms/test/extra/p2.test.4.out.pgm") == 0,
    "Output PGM should be correct."
  );
  freeCPGM(cpgm);
  freePGM(pgm);
  freePGM(pgm2);
}

int main() {
  // testP2Read();
  // testP2ReadWriteRead();
  // testP2Compress();
  // testP2Compress2();
  // testP2CPGMWriteReadDecompress();
  testP2ReplaceColorSingle();
  testP2ReplaceColorOccurences();
  testP2ReplaceColorFirstLast();
  testP2ReplaceColorReduceToOne();
  testP2ReplaceColorStandingLast();
  testP2SetColorSingular();
  testP2SetColorMergeLeftToNextFirst();
  testP2SetColorMergeLeftToNextMiddle();
  testP2SetColorMergeLeftToNextLast();
  testP2SetColorMergeRightToPrevFirst();
  testP2SetColorMergeRightToPrevMiddle();
  testP2SetColorMergeRightToPrevLast();
  testP2SetColorMergeMiddleWithSurrounding();
  testP2SetColorSplitEntry();
  testP2SetColorSplitMerge();
  testP2SomeExtraCases1();
  testP2SomeExtraCases2();
  testP2SomeExtraCases3();
  testP2SomeExtraCases4();

  printf("Success: %d (all) asserts are passed.\n", testCount);
  return 0;
}
