#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

/**
 * Program configuration definitions.
 */
#define LINESIZE 256
#define COLUMN_DELIMITER ","
#define NULCHR '\0'
#define NLCHR '\n'
#define NLSTR "\n"
#define INDENT "      "

/**
 * File name definitions.
 */
#define FILE_CONFIG "zdb-config.txt"
#define FILE_STUDENTS "zdb-students.txt"
#define FILE_COURSES "zdb-courses.txt"
#define FILE_LECTURERS "zdb-lecturers.txt"
#define FILE_ENROLLMENTS "zdb-enrollments.txt"

/**
 * Config keys.
 */
#define CKEY_SETUP "Setup Completed"
#define CKEY_MAX_COURSES "Max Courses"
#define CKEY_MAX_CREDITS "Max Credits"

/**
 * Domain specific type definitions & data structures.
 */
typedef enum {
  DROPPED,
  ENROLLED
} EnrollmentState;

typedef struct {
  int id;
  char* number;
  char* name;
  char* surname;
  int semester;
  int takenCredits;
  int takenCourses;
} Student;

typedef struct {
  int id;
  int lecturerId;
  char* code;
  char* name;
  int semester;
  int credit;
  int quota;
} Course;

typedef struct {
  int id;
  char* name;
  char* surname;
  char* title;
} Lecturer;

typedef struct {
  int id;
  int studentId;
  int courseId;
  char* date;
  EnrollmentState state;
} Enrollment;

/**
 * Helper type definitions & data structures.
 */
typedef struct {
  void** array;
  size_t length;
} DynamicArray;

typedef struct {
  char** array;
  size_t length;
} DynamicStringArray;

typedef struct {
  int** array;
  size_t length;
} DynamicIntArray;

typedef DynamicStringArray Columns;
typedef DynamicIntArray IdList;

typedef void (*Freer)(void*);
typedef void* (*CastColumns)(Columns*);
typedef char* (*CastToLine)(void*);
typedef bool (*PredicateColumns)(Columns*, char*);
typedef bool (*PredicateStr)(char*, char*);
typedef bool (*PredicateInt)(int, int);
typedef void (*Lister)(Columns*, bool);
typedef int (*Menu)();

/**
 * Common Generic Utilities
 */
bool inArrayInt(int* array, int size, int value) {
  int i = 0;
  while(i < size) if (array[i++] == value) return true;
  return false;
}

bool inArrayChr(char* array, int size, char value) {
  int i = 0;
  while(i < size) if (array[i++] == value) return true;
  return false;
}

char* mallocstr(int size) {
  return (char*) malloc((size + 1) * sizeof(char));
}

char* copystr(char* str) {
  char* copy = mallocstr(strlen(str));
  if (copy) strcpy(copy, str);
  return copy;
}

char* toStr(int num) {
  char* str = mallocstr(10);
  sprintf(str, "%d", num);
  return str;
}

bool isEmptyStr(char* str) {
  return str == NULL || strcmp(str, "") == 0;
}

void clearstr(char* str) {
  char allowed[] = {' ', '-', ':'};
  int i, j;
  char chr;

  i = j = 0;
  while ((chr = str[i++]) != NULCHR) {
    if (isalnum(chr) || inArrayChr(allowed, 3, chr)) {
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

char* scan(int size) {
  char* input = scanRaw(size);
  clearstr(input);
  return input;
}

int scanInt() {
  char* input = scan(10);
  int num = atoi(input);
  free(input); 
  return num;
}

char* scanStr(int size, char* defval) {
  char* input = scanRaw(size);

  if (strcmp(input, NLSTR) == 0) {
    strcpy(input, defval);
  } else if (strcmp(input, " \n") == 0) {
    strcpy(input, "");
  } else {
    clearstr(input);
  }

  return input;
}

char* promptStr(char* ask, int inputSize, char* defval) {
  char* output = mallocstr(strlen(ask));
  sprintf(output, ask, defval);
  printf("%s", output);
  free(output);
  
  return scanStr(inputSize, defval);
}

int promptInt(char* ask, int inputSize, int defval) {
  char* defvalStr = toStr(defval);
  char* input = promptStr(ask, inputSize, defvalStr);
  int inputInt = atoi(input);

  free(defvalStr);
  free(input);

  return inputInt;
}

char* promptStrWith(char* ask, int inputSize, char* defval, char* searchStr, PredicateStr predicate) {
  while (true) {
    char* input = promptStr(ask, inputSize, defval);
    if (predicate(input, searchStr)) {
      return input;
    }
    free(input);
  }
}

int promptIntWith(char* ask, int inputSize, int defval, int searchInt, PredicateInt predicate) {
  while (true) {
    int input = promptInt(ask, inputSize, defval);
    if (predicate(input, searchInt)) {
      return input;
    }
  }
}

void pressEnterToSeeMenu() {
  printf("Press enter to see menu...\n");  
  free(scanRaw(1));
}

Columns* parse(char* str) {
  char* string = copystr(str);
  char** array = (char**) malloc(sizeof(char *));

  int initialTokenSize = 8;
  int tokenSize;

  size_t columnIndex = 0;
  size_t charIndex = 0;
  size_t tokenIndex = 0;

  char* token;
  char chr;

  chr = string[0];
  tokenSize = initialTokenSize;

  while (chr != NULCHR) {
    if (tokenIndex == 0) {
      tokenSize = initialTokenSize;
      array = (char**) realloc(array, (columnIndex + 1) * sizeof(char *));
      array[columnIndex] = (char*) malloc(tokenSize * sizeof(char));
      token = array[columnIndex];
      *token = NULCHR;
    }

    if (tokenIndex >= tokenSize - 1) {
      tokenSize = tokenSize * 2;
      array[columnIndex] = (char*) realloc(array[columnIndex], tokenSize * sizeof(char));
      token = array[columnIndex];
    }

    if (chr == COLUMN_DELIMITER[0]) {
      columnIndex++;
      tokenIndex = 0;
      clearstr(token);
    } else {
      *(token + tokenIndex) = chr;
      *(token + tokenIndex + 1) = NULCHR;
      tokenIndex++;
    }

    chr = string[++charIndex];
  }
  
  clearstr(array[columnIndex]);
  free(string);

  Columns* columns = (Columns*) malloc(sizeof(Columns));
  columns->array = array;
  columns->length = columnIndex + 1;

  return columns;
}

IdList* filterIdList(IdList* idList, int searchInt, PredicateInt predicate) {
  int** array = (int**) malloc(sizeof(int *));
  int index = 0;

  int i;
  for (i = 0; i < idList->length; i++) {
    int id = *idList->array[i];
    if (predicate(id, searchInt)) {
      array = (int**) realloc(array, (index + 1) * sizeof(int *));
      array[index] = (int*) malloc(sizeof(int));
      *array[index] = id;
      index++;
    }
  }

  IdList* newIdList = (IdList*) malloc(sizeof(IdList));
  newIdList->array = array;
  newIdList->length = index;

  return newIdList;
}

/**
 * Common data structure operations.
 */
void freeDynamicArray(void* data) {
  DynamicArray* darray = data;
  int i;
  for (i = 0; i < darray->length; i++) {
    free(darray->array[i]);
  }
  free(darray->array);
  free(darray);
}

void freeColumns(Columns* columns) {
  freeDynamicArray(columns);
}

void freeIdList(IdList* idList) {
  freeDynamicArray(idList);
}

char* now() {
  char* timeStr = mallocstr(20);
  time_t tTime = time(NULL);
  struct tm t = *localtime(&tTime);
  sprintf(
    timeStr, "%d-%02d-%02d %02d:%02d:%02d",
    t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec
  );
  return timeStr;
}

/**
 * Generic & Domain Specific File Operations and
 * Database helpers
 */
bool columnEqualityTester(Columns* columns, char* searchStr) {
  Columns* search = parse(searchStr);
  int matchColumn = atoi(search->array[0]);
  char* matchKey = search->array[1];
  int compare = strcmp(columns->array[matchColumn], matchKey);
  freeColumns(search);
  return compare == 0;
}

char* buildSearchStr(int columnIndex, char* matchKey) {
  char* searchStr = mallocstr(strlen(matchKey) + 5);
  sprintf(searchStr, "%d%s%s", columnIndex, COLUMN_DELIMITER, matchKey);
  return searchStr;
}

bool predicateColumnsAll(Columns* columns, char* searchStr) {
  return true;
}

bool createFile(char* name) {
  FILE* file = fopen(name, "a+");
  if (file == NULL) {
    printf("Error: Could not create file named '%s'.\n", name);
    return false;
  }
  fclose(file);
  return true;
}

FILE* openFile(char* filename, char* as) {
  FILE* file = fopen(filename, as);
  if (file == NULL) {
    printf("Error: Could not open file; name=%s, mode=%s", filename, as);
    return NULL;
  };
  return file;
}

Columns* getLastRecord(char* filename) {
  FILE* file = fopen(filename, "r");
  if (file==NULL) return NULL;

  char* line = mallocstr(LINESIZE);
  
  int i = 0;
  while (fgets(line, LINESIZE, file)) {
    i++;
  }
  fclose(file);

  if (i == 0) return NULL;
  Columns* columns = parse(line);
  free(line); 
  return columns;
}

int getMaxId(char* filename) {
  Columns* lastRecord = getLastRecord(filename);
  if (lastRecord == NULL) return 0;
  int id = atoi(lastRecord->array[0]); 
  freeColumns(lastRecord);
  return id;
}

bool fileHasRecord(char* filename) {
  FILE* file = fopen(filename, "r");
  if (file==NULL) return false;
  char* line = mallocstr(LINESIZE);
  clearstr(line);
  fgets(line, LINESIZE, file);
  bool hasRecord = !isEmptyStr(line);
  free(line);
  fclose(file);
  return hasRecord;
}

IdList* findAllRecords(char* filename, char* searchStr, PredicateColumns predicate) {
  FILE* file = fopen(filename, "r");
  if (file==NULL) return NULL;

  int** array = (int**) malloc(sizeof(int *));
  int index = 0;

  char* line = mallocstr(LINESIZE);

  while (fgets(line, LINESIZE, file)) {
    Columns* columns = parse(line);
    bool match = predicate(columns, searchStr);

    if (match) {
      array = (int**) realloc(array, (index + 1) * sizeof(int *));
      array[index] = (int*) malloc(sizeof(int));
      *array[index] = atoi(columns->array[0]);
      index++;
    }

    freeColumns(columns);
  }

  IdList* idList = (IdList*) malloc(sizeof(IdList));
  idList->array = array;
  idList->length = index;

  free(line);
  fclose(file);
  return idList;
}

IdList* findAllRecordsByColumn(char* filename, int column, char* match) {
  char* searchStr = buildSearchStr(column, match);
  IdList* idList = findAllRecords(filename, searchStr, columnEqualityTester);
  free(searchStr);
  if (idList == NULL) return NULL;
  return idList;
}

IdList* findAllRecordsByIntColumn(char* filename, int column, int match) {
  char* matchStr = toStr(match);
  IdList* idList = findAllRecordsByColumn(filename, column, matchStr);
  free(matchStr);
  return idList;
}

Columns* findRecord(char* filename, char* searchStr, PredicateColumns predicate) {
  FILE* file = fopen(filename, "r");
  if (file==NULL) return NULL;

  Columns* columns;
  char* line = mallocstr(LINESIZE);

  while (fgets(line, LINESIZE, file)) {
    columns = parse(line);

    if (predicate(columns, searchStr)) {
      free(line);
      fclose(file);
      return columns;
    }

    freeColumns(columns);
  }

  free(line);
  fclose(file);
  return NULL;
}

Columns* findRecordByColumn(char* filename, int column, char* match) {
  char* searchStr = buildSearchStr(column, match);
  Columns* columns = findRecord(filename, searchStr, columnEqualityTester);
  free(searchStr);
  if (columns == NULL) return NULL;
  return columns;
}

Columns* findRecordByIntColumn(char* filename, int column, int match) {
  char* matchStr = toStr(match);
  Columns* columns = findRecordByColumn(filename, column, matchStr);
  free(matchStr);
  return columns;
}

void* findCastRecordByColumn(char* filename, int column, char* match, CastColumns caster) {
  Columns* columns = findRecordByColumn(filename, column, match);
  if (columns == NULL) return NULL;
  void* casted = caster(columns);
  freeColumns(columns);
  return casted;
}

void* findCastRecordByIntColumn(char* filename, int column, int match, CastColumns caster) {
  char* matchStr = toStr(match);
  void* record = findCastRecordByColumn(filename, column, matchStr, caster);
  free(matchStr);
  return record;
}

void updateRecord(char* filename, char* updated, char* searchStr, PredicateColumns predicate) {
  char tmpFile[strlen(filename) + 4 + 1];
  strcpy(tmpFile, filename);
  strcat(tmpFile, "-tmp");
  FILE* file = fopen(filename, "r");
  FILE* tmp = fopen(tmpFile, "a+");

  char* line = mallocstr(LINESIZE);
  while (fgets(line, LINESIZE, file)) {
    Columns* columns = parse(line);
    bool match = predicate(columns, searchStr);
    freeColumns(columns);

    if (!match) {
      fputs(line, tmp);
    } else if (updated != NULL) {
      fputs(updated, tmp);
    }
  }

  fclose(file);
  fclose(tmp);
  free(line);

  remove(filename);
  rename(tmpFile, filename);
}

void updateRecordByColumn(char* filename, int column, char* match, char* updated) {
  char* searchStr = buildSearchStr(column, match);
  updateRecord(filename, updated, searchStr, columnEqualityTester);
  free(searchStr);
}

void updateCastRecordByColumn(char* filename, int column, char* match, void* data, CastToLine caster) {
  char* line = (data != NULL) ? caster(data) : NULL;
  updateRecordByColumn(filename, column, match, line);
  free(line);
}

void updateCastRecordByIntColumn(char* filename, int column, int match, void* data, CastToLine caster) {
  char* matchStr = toStr(match);
  updateCastRecordByColumn(filename, column, matchStr, data, caster);
  free(matchStr);
}

bool insertRecord(char* filename, void* data, CastToLine caster) {
  FILE* file = openFile(filename, "a+");
  if (file == NULL) return false;

  char* line = caster(data);
  fputs(line, file);

  free(line);
  fclose(file);
  return true;
}

void listRecords(char* filename, Lister lister, char* searchStr, PredicateColumns predicate) {
  FILE* file = openFile(filename, "r");
  char* line = mallocstr(LINESIZE);

  bool head = true;
  while (fgets(line, LINESIZE, file)) {
    Columns* columns = parse(line);
    if (predicate(columns, searchStr)) {
      lister(columns, head);
      head = false;
    }
    freeColumns(columns);
  }

  if (head) {
    printf("\nNo records found.\n");
  }
  printf("\n");

  fclose(file);
  free(line);

  pressEnterToSeeMenu();
}

int readConfig(char* key) {
  Columns* columns = findRecordByColumn(FILE_CONFIG, 0, key);
  if (columns == NULL) return -1;
  int value = atoi(columns->array[1]);
  freeColumns(columns);
  return value;
}

/**
 * UI Helpers
 */
void printRepeated(char chr, int repeat) {
  while (repeat--) printf("%c", chr);
  printf("\n");
}

void printSeparator(int repeat) {
  printRepeated('-', repeat);
}

void printPromptHeader(char* title, int mode) {
  printSeparator(33);
  printf(INDENT"%s\n", title);

  if (mode == 0) { // new
    printf(INDENT"* Press enter to skip.\n");
  }

  if (mode == 1) { // edit
    printf(INDENT"* Press enter to keep current value as it is.\n");
    printf(INDENT"* Input one space to clear current value.\n");
  }
}

/**
 * Some Entity Operation Allusions
 */
IdList* findCoursesByLecturerId(int lecturerId);
Course* findCourseById(int id);
Course* findCourseByCode(char* code);
bool coursesHasRecord();
bool courseExistanceByCodeTester(char* courseCode, char* searchStr);
void freeCourse(void* data);
void deleteEnrollmentsByStudentId(int studentId);
void setCourseLecturerId(int courseId, int lecturerId);

/**
 * Entity operation helpers.
 */
bool existanceTestHelper(void* data, Freer freer) {
  if (data == NULL) return false;
  freer(data);
  return true;
}

/**
 * Lecturer Operations
 */
Lecturer* newLecturer() {
  Lecturer* lecturer = (Lecturer*) malloc(sizeof(Lecturer));
  if (lecturer == NULL) {
    printf("Error: Cannot allocate memory for a lecturer.\n");
    return NULL;
  }
  return lecturer;
}

void* columnsToLecturer(Columns* columns) {
  Lecturer* lecturer = newLecturer();
  if (lecturer == NULL) return NULL;

  lecturer->id = atoi(columns->array[0]);
  lecturer->name = copystr(columns->array[1]);
  lecturer->surname = copystr(columns->array[2]);
  lecturer->title = copystr(columns->array[3]);

  return lecturer;
}

char* lecturerToLine(void* data) {
  Lecturer* lecturer = data;
  char* line = mallocstr(LINESIZE);
  sprintf(
    line,
    "%d%s%s%s%s%s%s\n",
    lecturer->id, COLUMN_DELIMITER,
    lecturer->name, COLUMN_DELIMITER,
    lecturer->surname, COLUMN_DELIMITER,
    lecturer->title
  );

  return line;
}

void freeLecturer(void* data) {
  Lecturer* lecturer = data;
  free(lecturer->name);
  free(lecturer->surname);
  free(lecturer->title);
  free(lecturer);
}

Lecturer* findLecturerByColumn(int column, char* match) {
  return findCastRecordByColumn(FILE_LECTURERS, column, match, columnsToLecturer);
}

Lecturer* findLecturerByIntColumn(int column, int match) {
  return findCastRecordByIntColumn(FILE_LECTURERS, column, match, columnsToLecturer);
}

Lecturer* findLecturerById(int id) {
  return findLecturerByIntColumn(0, id);
}

void updateLecturerById(int id, Lecturer* lecturer) {
  updateCastRecordByIntColumn(FILE_LECTURERS, 0, id, lecturer, lecturerToLine);
}

bool lecturersHasRecord() {
  return fileHasRecord(FILE_LECTURERS);
}

bool isLecturerExistById(int lecturerId) {
  return existanceTestHelper(findLecturerById(lecturerId), freeLecturer);
}

bool lecturerExistanceByIdTester(int lecturerId, int searchInt) {
  if (lecturerId == 0) return true;
  bool exist = isLecturerExistById(lecturerId);
  if (!exist) {
    printf(INDENT"Error: There is no lecturer with id #%d, press enter to skip.\n", lecturerId);
  }
  return exist;
}

int deassignLecturerFromCourses(int lecturerId) {
  IdList* lecturerCourses = findCoursesByLecturerId(lecturerId);
  int courseIndex = 0;
  for (courseIndex = 0; courseIndex<lecturerCourses->length; courseIndex++) {
    setCourseLecturerId(*lecturerCourses->array[courseIndex], 0);
  }
  freeIdList(lecturerCourses);
  return courseIndex;
}

char* buildFullLecturerInfo(Lecturer* lecturer) {
  char* fullLecturerInfo = mallocstr(40);
  sprintf(
    fullLecturerInfo, "%s. %s %s (#%d)",
    lecturer->title, lecturer->name, lecturer->surname, lecturer->id
  );
  return fullLecturerInfo;
}

void lecturerLister(Columns* columns, bool head) {
  Lecturer* lecturer = columnsToLecturer(columns);
  
  if (head) {
    printSeparator(53);
    printf("%-5s %-15s %-15s %-15s\n", "id", "name", "surname", "title");
    printSeparator(53);
  }

  printf(
    "%-5d %-15s %-15s %-15s\n",
    lecturer->id,
    lecturer->name,
    lecturer->surname,
    lecturer->title
  );

  freeLecturer(lecturer);
} 

int listLecturers() {
  listRecords(FILE_LECTURERS, lecturerLister, "", predicateColumnsAll);
  return 1;
}

int addLecturer() {
  Lecturer* lecturer = newLecturer();
  if (lecturer == NULL) return 0;

  printPromptHeader("New Lecturer", 0);

  lecturer->name = promptStr("(1/3) Name: ", 15, "");
  lecturer->surname = promptStr("(2/3) Surname: ", 15, "");
  lecturer->title = promptStr("(3/3) Title: ", 15, "");
  lecturer->id = getMaxId(FILE_LECTURERS) + 1;

  bool inserted = insertRecord(FILE_LECTURERS, lecturer, lecturerToLine);
  if (inserted) {
    printf(
      "\nLecturer (#%d) %s. %s %s added.\n",
      lecturer->id,
      lecturer->title,
      lecturer->name,
      lecturer->surname
    );
  }

  freeLecturer(lecturer);
  pressEnterToSeeMenu();
  return 1;
}

int editLecturer() {
  printf("\nEnter lecturer id to edit (press enter to cancel): ");

  int lecturerId = promptIntWith("\n"INDENT"Lecturer id: ", LINESIZE, 0, 0, lecturerExistanceByIdTester);
  if (lecturerId == 0) return 1;
  Lecturer* lecturer = findLecturerById(lecturerId);

  printPromptHeader("Edit Lecturer", 1);

  lecturer->name = promptStr("(1/3) Name (%s): ", 15, lecturer->name);
  lecturer->surname = promptStr("(2/3) Surname (%s): ", 15, lecturer->surname);
  lecturer->title = promptStr("(3/3) Title (%s): ", 15, lecturer->title);

  updateLecturerById(lecturerId, lecturer);
  printf("\nLecturer #%d updated.\n", lecturerId);

  freeLecturer(lecturer);
  pressEnterToSeeMenu();
  return 1;
}

int deleteLecturer() {
  printf("\nEnter lecturer id to delete (press enter to cancel): ");

  int lecturerId = promptIntWith("\n"INDENT"Lecturer id: ", LINESIZE, 0, 0, lecturerExistanceByIdTester);
  if (lecturerId == 0) return 1;
  Lecturer* lecturer = findLecturerById(lecturerId);

  updateLecturerById(lecturerId, NULL);
  printf("\nLecturer #%d deleted.\n", lecturerId);

  int lecturerCourses = deassignLecturerFromCourses(lecturerId);
  printf("Lecturer deassigned from %d courses.\n", lecturerCourses);

  freeLecturer(lecturer);
  pressEnterToSeeMenu();
  return 1;
}

/**
 * Student operations.
 */
Student* newStudent() {
  Student* student = (Student*) malloc(sizeof(Student));
  if (student == NULL) {
    printf("Error: Cannot allocate memory for a student.\n");
    return NULL;
  }
  return student;
}

void* columnsToStudent(Columns* columns) {
  Student* student = newStudent();
  if (student == NULL) return NULL;

  student->id = atoi(columns->array[0]);
  student->number = copystr(columns->array[1]);
  student->name = copystr(columns->array[2]);
  student->surname = copystr(columns->array[3]);
  student->semester = atoi(columns->array[4]);
  student->takenCredits = atoi(columns->array[5]);
  student->takenCourses = atoi(columns->array[6]);

  return student;
}

char* studentToLine(void* data) {
  Student* student = data;
  char* line = mallocstr(LINESIZE);
  sprintf(
    line,
    "%d%s%s%s%s%s%s%s%d%s%d%s%d\n",
    student->id, COLUMN_DELIMITER,
    student->number, COLUMN_DELIMITER,
    student->name, COLUMN_DELIMITER,
    student->surname, COLUMN_DELIMITER,
    student->semester, COLUMN_DELIMITER,
    student->takenCredits, COLUMN_DELIMITER,
    student->takenCourses
  );

  return line;
}

void freeStudent(void* data) {
  Student* student = data;
  free(student->number);
  free(student->name);
  free(student->surname);
  free(student);
}

Student* findStudentByColumn(int column, char* match) {
  return findCastRecordByColumn(FILE_STUDENTS, column, match, columnsToStudent);
}

Student* findStudentByIntColumn(int column, int match) {
  return findCastRecordByIntColumn(FILE_STUDENTS, column, match, columnsToStudent);
}

Student* findStudentById(int id) {
  return findStudentByIntColumn(0, id);
}

Student* findStudentByNumber(char* number) {
  return findStudentByColumn(1, number);
}

void updateStudentById(int id, Student* student) {
  updateCastRecordByIntColumn(FILE_STUDENTS, 0, id, student, studentToLine);
}

bool studentsHasRecord() {
  return fileHasRecord(FILE_STUDENTS);
}

bool isStudentExistById(int studentId) {
  return existanceTestHelper(findStudentById(studentId), freeStudent);
}

bool isStudentExistByNumber(char* studentNumber) {
  return existanceTestHelper(findStudentByNumber(studentNumber), freeStudent);
}

bool studentExistanceByIdTester(int studentId, int searchInt) {
  if (studentId == 0) return true;
  bool exist = isStudentExistById(studentId);
  if (!exist) printf(INDENT"Error: There is no student with id #%d, press enter to skip.\n", studentId);
  return exist;
}

bool studentExistanceByNumberTester(char* studentNumber, char* searchStr) {
  if (strcmp(studentNumber, "") == 0) return true;
  bool exist = isStudentExistByNumber(studentNumber);
  if (!exist) printf(INDENT"Error: There is no student with number #%s, press enter to skip.\n", studentNumber);
  return exist;
}

void studentLister(Columns* columns, bool head) {
  Student* student = columnsToStudent(columns);
  
  if (head) {
    printSeparator(101);
    printf(
      "%-5s %-15s %-15s %-15s %-15s %-15s %-15s\n",
      "id", "number", "name", "surname", "semester", "credits", "courses"
    );
    printSeparator(101);
  }

  printf(
    "%-5d %-15s %-15s %-15s %-15d %-15d %-15d\n",
    student->id,
    student->number,
    student->name,
    student->surname,
    student->semester,
    student->takenCredits,
    student->takenCourses
  );

  freeStudent(student);
} 

int listStudents() {
  listRecords(FILE_STUDENTS, studentLister, "", predicateColumnsAll);
  return 1;
}

bool studentNumberUniqTester(char* number, char* currentNumber) {
  if (strcmp(number, currentNumber) == 0) return true;
  Student* student = findStudentByNumber(number);

  if (student != NULL) {
    printf(INDENT"Error: Student exist with number %s, please enter different one.\n", number);
    freeStudent(student);
    return false;
  }

  return true;
}

int addStudent() {
  Student* student = newStudent();
  if (student == NULL) return 0;

  printPromptHeader("New Student", 0);

  student->number = promptStrWith("(1/4) Number: ", 15, "", "", studentNumberUniqTester);
  student->name = promptStr("(2/4) Name: ", 15, "");
  student->surname = promptStr("(3/4) Surname: ", 15, "");
  student->semester = promptInt("(4/4) Semester: ", 2, 1);
  student->takenCredits = 0;
  student->takenCourses = 0;
  student->id = getMaxId(FILE_STUDENTS) + 1;

  bool inserted = insertRecord(FILE_STUDENTS, student, studentToLine);
  if (inserted) {
    printf(
      "\nStudent (#%d) %s %s %s added.\n",
      student->id,
      student->number,
      student->name,
      student->surname
    );
  }

  freeStudent(student);
  pressEnterToSeeMenu();
  return 1;
}

int editStudent() {
  printf("\nEnter student id to edit (press enter to cancel): ");

  int studentId = promptIntWith("\n"INDENT"Student id: ", LINESIZE, 0, 0, studentExistanceByIdTester);
  if (studentId == 0) return 1;
  Student* student = findStudentById(studentId);

  printPromptHeader("Edit Student", 1);

  student->number = promptStrWith("(1/4) Number (%s): ", 15, student->number, student->number, studentNumberUniqTester);
  student->name = promptStr("(2/4) Name (%s): ", 15, student->name);
  student->surname = promptStr("(3/4) Surname (%s): ", 15, student->surname);
  student->semester = promptInt("(4/4) Semester (%s): ", 2, student->semester);
  
  updateStudentById(studentId, student);
  printf("\nStudent #%d updated.\n", studentId);

  freeStudent(student);
  pressEnterToSeeMenu();
  return 1;
}

int deleteStudent() {
  printf("\nEnter student id to delete (press enter to cancel): ");
  
  int studentId = promptIntWith("\n"INDENT"Student id: ", LINESIZE, 0, 0, studentExistanceByIdTester);
  if (studentId == 0) return 1;
  Student* student = findStudentById(studentId);

  deleteEnrollmentsByStudentId(studentId);
  updateStudentById(studentId, NULL);
  printf("\nStudent #%d deleted.\n", studentId);

  freeStudent(student); 
  pressEnterToSeeMenu();
  return 1;
}

/**
 * Enrollment Operations
 */
Enrollment* newEnrollment() {
  Enrollment* enrollment = (Enrollment*) malloc(sizeof(Enrollment));
  if (enrollment == NULL) {
    printf("Error: Cannot allocate memory for a enrollment.\n");
    return NULL;
  }
  return enrollment;
}

void* columnsToEnrollment(Columns* columns) {
  Enrollment* enrollment = newEnrollment();
  if (enrollment == NULL) return NULL;

  enrollment->id = atoi(columns->array[0]);
  enrollment->studentId = atoi(columns->array[1]);
  enrollment->courseId = atoi(columns->array[2]);
  enrollment->date = copystr(columns->array[3]);
  enrollment->state = atoi(columns->array[4]);

  return enrollment;
}

char* enrollmentToLine(void* data) {
  Enrollment* enrollment = data;
  char* line = mallocstr(LINESIZE);
  sprintf(
    line,
    "%d%s%d%s%d%s%s%s%d\n",
    enrollment->id, COLUMN_DELIMITER,
    enrollment->studentId, COLUMN_DELIMITER,
    enrollment->courseId, COLUMN_DELIMITER,
    enrollment->date, COLUMN_DELIMITER,
    enrollment->state
  );

  return line;
}

void freeEnrollment(void* data) {
  Enrollment* enrollment = data;
  free(enrollment->date);
  free(enrollment);
}

Enrollment* findEnrollmentByColumn(int column, char* match) {
  return findCastRecordByColumn(FILE_ENROLLMENTS, column, match, columnsToEnrollment);
}

Enrollment* findEnrollmentByIntColumn(int column, int match) {
  return findCastRecordByIntColumn(FILE_ENROLLMENTS, column, match, columnsToEnrollment);
}

IdList* findAllEnrollmentsByIntColumn(int column, int match) {
  return findAllRecordsByIntColumn(FILE_ENROLLMENTS, column, match);
}

Enrollment* findEnrollmentById(int id) {
  return findEnrollmentByIntColumn(0, id);
}

IdList* findAllEnrollmentsByStudentId(int studentId) {
  return findAllEnrollmentsByIntColumn(1, studentId);
}

IdList* findAllEnrollmentsByCourseId(int courseId) {
  return findAllEnrollmentsByIntColumn(2, courseId);
}

bool activeEnrollmentTester(int id, int searchInt) {
  Enrollment* enrollment = findEnrollmentById(id);
  bool enrolled = enrollment->state == ENROLLED;
  freeEnrollment(enrollment);
  return enrolled;
}

IdList* findAllActiveEnrollmentsByCourseId(int courseId) {
  IdList* enrollments = findAllEnrollmentsByCourseId(courseId);
  if (enrollments == NULL) return NULL;
  IdList* activeEnrollments = filterIdList(enrollments, 0, activeEnrollmentTester);
  freeIdList(enrollments);
  return activeEnrollments;
}

Enrollment* findEnrollmentByStudentNCourse(int studentId, int courseId) {
  IdList* enrollments = findAllEnrollmentsByStudentId(studentId);

  int i;
  for (i = 0; i < enrollments->length; i++) {
    Enrollment* enrollment = findEnrollmentById(*enrollments->array[i]);
    if (enrollment->courseId == courseId) {
      freeIdList(enrollments);
      return enrollment;
    }
    freeEnrollment(enrollment);
  }

  freeIdList(enrollments);
  return NULL;
}

void updateEnrollmentById(int id, Enrollment* enrollment) {
  updateCastRecordByIntColumn(FILE_ENROLLMENTS, 0, id, enrollment, enrollmentToLine);
}

void bulkDeleteEnrollments(IdList* idList) {
  int i;
  for (i = 0; i < idList->length; i++) {
    updateEnrollmentById(*idList->array[i], NULL);
  }
}

void deleteEnrollmentsByStudentId(int studentId) {
  IdList* idList = findAllEnrollmentsByStudentId(studentId);
  bulkDeleteEnrollments(idList);
  freeIdList(idList);
}

void deleteEnrollmentsByCourseId(int courseId) {
  IdList* idList = findAllEnrollmentsByCourseId(courseId);

  int i;
  for (i = 0; i < idList->length; i++) {
    Enrollment* enrollment = findEnrollmentById(*idList->array[i]);
    if (enrollment->state == ENROLLED) {
      Student* student = findStudentById(enrollment->studentId);
      Course* course = findCourseById(enrollment->courseId);
      student->takenCourses -= 1;
      student->takenCredits -= course->credit;
      updateStudentById(student->id, student);
      freeCourse(course);
      freeStudent(student);
    }
    freeEnrollment(enrollment);
  }

  bulkDeleteEnrollments(idList);
  freeIdList(idList);
}

void enrollmentLister(Columns* columns, bool head) {
  if (head) {
    printSeparator(124);
    printf(
      "%-47s %-46s %-20s %-10s\n",
      "student", "course", "date", "state"
    );
    printSeparator(124);
  }

  Enrollment* enrollment = columnsToEnrollment(columns);
  Student* student = findStudentById(enrollment->studentId);
  Course* course = findCourseById(enrollment->courseId);

  char* enrollmentState = enrollment->state == ENROLLED ?
    "Enrolled" : "Dropped";

  char* studentInfo = mallocstr(47);
  sprintf(studentInfo, "%-.15s %-.15s %-.15s", student->number, student->name, student->surname);
  
  char* courseInfo = mallocstr(46);
  sprintf(courseInfo, "%-.15s %-.30s", course->code, course->name);

  printf(
    "%-47s %-46s %-20s %-10s\n",
    studentInfo, courseInfo,
    enrollment->date, enrollmentState
  );

  free(studentInfo);
  free(courseInfo);
  freeEnrollment(enrollment);
  freeStudent(student);
  freeCourse(course);
}

int listAllEnrollments() {
  listRecords(FILE_ENROLLMENTS, enrollmentLister, "", predicateColumnsAll);
  return 1;
}

int listEnrolledCoursesOfStudent() {
  if (!studentsHasRecord()) {
    printf("\nThere are no students available.\n");
    pressEnterToSeeMenu();
    return 1;
  }

  printf("\nEnter student number to list enrolled courses.\n");
  char* studentNumber = promptStrWith("Student number: ", LINESIZE, "", "", studentExistanceByNumberTester);
  if (isEmptyStr(studentNumber)) return 1;
  Student* student = findStudentByNumber(studentNumber);
  
  printf("\n");

  char* strStudentId = toStr(student->id);
  char* searchStr = buildSearchStr(1, strStudentId);
  listRecords(FILE_ENROLLMENTS, enrollmentLister, searchStr, columnEqualityTester);
  
  free(studentNumber);
  free(strStudentId);
  free(searchStr);
  freeStudent(student);
  return 1;
}

int listEnrolledStudentsOfCourse() {
  if (!coursesHasRecord()) {
    printf("\nThere are no courses available.\n");
    pressEnterToSeeMenu();
    return 1;
  }

  printf("\nEnter course code to list enrolled students.\n");
  char* courseCode = promptStrWith("Course code: ", LINESIZE, "", "", courseExistanceByCodeTester);
  if (isEmptyStr(courseCode)) return 1;;
  Course* course = findCourseByCode(courseCode);

  printf("\n");

  char* strCourseId = toStr(course->id);
  char* searchStr = buildSearchStr(2, strCourseId);
  listRecords(FILE_ENROLLMENTS, enrollmentLister, searchStr, columnEqualityTester);
  
  free(courseCode);
  free(strCourseId);
  free(searchStr);
  freeCourse(course);
  return 1;
}

int exportEnrolledStudentsOfCourse() {
  if (!coursesHasRecord()) {
    printf("\nThere are no courses available.\n");
    pressEnterToSeeMenu();
    return 1;
  }

  printf("\nEnter course code to list enrolled students.\n");
  char* courseCode = promptStrWith("Course code: ", LINESIZE, "", "", courseExistanceByCodeTester);
  if (isEmptyStr(courseCode)) return 1;;
  Course* course = findCourseByCode(courseCode);

  IdList* activeEnrollments = findAllActiveEnrollmentsByCourseId(course->id);
  if (activeEnrollments == NULL || activeEnrollments->length == 0) {
    printf("\nThere is no enrollment for this course at the moment.\n");
    free(courseCode);
    freeCourse(course);
    pressEnterToSeeMenu();
    return 1;  
  }

  char* suffix = "_class.txt";
  char* filename = mallocstr(strlen(courseCode) + strlen(suffix));
  sprintf(filename, "%s%s", courseCode, suffix);
  remove(filename);

  FILE* file = fopen(filename, "a+");
  int i;
  for (i = 0; i < activeEnrollments->length; i++) {
    Enrollment* enrollment = findEnrollmentById(*activeEnrollments->array[i]);
    Student* student = findStudentById(enrollment->studentId);
    
    fprintf(file, "%s %s %s\n", student->number, student->name, student->surname);
    
    freeStudent(student);
    freeEnrollment(enrollment);
  }

  printf("\nThere is %zu enrollment for %s course.\n", activeEnrollments->length, courseCode);
  printf("Enrolled students exported into file named (%s).\n\n", filename);

  fclose(file);
  free(filename);
  free(courseCode);
  freeCourse(course);
  pressEnterToSeeMenu();
  return 1;
}

int enrollStudent() {
  if (!coursesHasRecord()) {
    printf("\nThere are no courses available.\n");
    pressEnterToSeeMenu();
    return 1;
  }

  if (!studentsHasRecord()) {
    printf("\nThere are no students available.\n");
    pressEnterToSeeMenu();
    return 1;
  }

  printf("\n"INDENT"Course enrollment\n");
  printf("\n"INDENT"Enter student number (press enter to cancel): ");
  
  int allowedMaxCourses = readConfig(CKEY_MAX_COURSES);
  int allowedMaxCredits = readConfig(CKEY_MAX_CREDITS);

  char* studentNumber = promptStrWith("\n"INDENT"Student number: ", LINESIZE, "", "", studentExistanceByNumberTester);
  if (isEmptyStr(studentNumber)) return 1;
  Student* student = findStudentByNumber(studentNumber);
  printf(INDENT"Student %s %s selected.\n", student->name, student->surname);
  free(studentNumber);

  // Did student reach course limit?
  if (student->takenCourses >= allowedMaxCourses) {
    printf("\nStudent %s cannot enroll in new courses since allowed maximum course number (%d) reached.\n", student->number, allowedMaxCourses);
    freeStudent(student);
    pressEnterToSeeMenu();
    return 1;
  }

  // Did student reached credit limit?
  if (student->takenCredits >= allowedMaxCredits) {
    printf("\nStudent %s cannot enroll in new courses since allowed maximum credits amount (%d) reached.\n", student->number, allowedMaxCredits);
    freeStudent(student);
    pressEnterToSeeMenu();
    return 1;
  }

  char* courseCode = promptStrWith("\n"INDENT"Course code: ", LINESIZE, "", "", courseExistanceByCodeTester);
  if (isEmptyStr(courseCode)) {
    freeStudent(student);
    return 1;
  };
  Course* course = findCourseByCode(courseCode);
  printf(INDENT"Course %s %s selected.\n", course->code, course->name);
  free(courseCode);

  // Is student already enrolled?
  Enrollment* enrollment = findEnrollmentByStudentNCourse(student->id, course->id);
  
  if (enrollment && enrollment->state == ENROLLED) {
    printf("\nStudent %s already enrolled to course %s.\n", student->number, course->code);
    freeStudent(student);
    freeCourse(course);
    freeEnrollment(enrollment);
    pressEnterToSeeMenu();
    return 1;
  }

  bool enrolledBefore = false;
  if (enrollment == NULL) {
    enrollment = newEnrollment();
    enrollment->studentId = student->id;
    enrollment->courseId = course->id;
    enrollment->id = getMaxId(FILE_ENROLLMENTS) + 1;
  } else {
    enrolledBefore = true;
  }

  enrollment->date = now();
  enrollment->state = ENROLLED;

  // Does course have quota?
  IdList* courseEnrollments = findAllActiveEnrollmentsByCourseId(course->id);
  if (courseEnrollments && courseEnrollments->length >= course->quota) {
    printf("\nCourse %s not available for new enrollments since it's full.\n", course->code);
    freeStudent(student);
    freeCourse(course);
    freeEnrollment(enrollment);
    freeIdList(courseEnrollments);
    pressEnterToSeeMenu();
    return 1;
  }

  // Does student have enough available credit?  
  if (student->takenCredits + course->credit > allowedMaxCredits) {
    printf("\nStudent %s cannot enroll in due to not having enough credit space.\n", student->number);
    freeStudent(student);
    freeCourse(course);
    freeEnrollment(enrollment);
    freeIdList(courseEnrollments);
    pressEnterToSeeMenu();
    return 1;
  }

  // Enroll
  if (enrolledBefore) {
    updateEnrollmentById(enrollment->id, enrollment);
  } else {
    insertRecord(FILE_ENROLLMENTS, enrollment, enrollmentToLine);
  }

  // Update student.
  student->takenCourses += 1;
  student->takenCredits += course->credit;
  updateStudentById(student->id, student);

  printf("\nStudent with number %s enrolled to the course with code %s.\n", student->number, course->code);
  
  freeStudent(student);
  freeCourse(course);
  freeEnrollment(enrollment);
  freeIdList(courseEnrollments);
  pressEnterToSeeMenu();
  return 1;
}

int dropEnrolledCourse() {
  if (!coursesHasRecord()) {
    printf("\nThere are no courses available.\n");
    pressEnterToSeeMenu();
    return 1;
  }

  if (!studentsHasRecord()) {
    printf("\nThere are no students available.\n");
    pressEnterToSeeMenu();
    return 1;
  }

  printf("\n"INDENT"Drop course enrollment\n");
  printf("\n"INDENT"Enter student number (press enter to cancel): ");

  char* studentNumber = promptStrWith("\n"INDENT"Student number: ", LINESIZE, "", "", studentExistanceByNumberTester);
  if (isEmptyStr(studentNumber)) return 1;
  Student* student = findStudentByNumber(studentNumber);
  printf(INDENT"Student %s %s selected.\n", student->name, student->surname);
  free(studentNumber);

  char* courseCode = promptStrWith("\n"INDENT"Course code: ", LINESIZE, "", "", courseExistanceByCodeTester);
  if (isEmptyStr(courseCode)) {
    freeStudent(student);
    return 1;
  };
  Course* course = findCourseByCode(courseCode);
  printf(INDENT"Course %s %s selected.\n", course->code, course->name);
  free(courseCode);

  Enrollment* enrollment = findEnrollmentByStudentNCourse(student->id, course->id);

  if (enrollment == NULL) {
    printf("\nStudent %s never enrolled to course %s.\n", student->number, course->code);
  } else if (enrollment->state == DROPPED) {
    printf("\nStudent %s already dropped course %s.\n", student->number, course->code);
    freeEnrollment(enrollment);
  } else if (enrollment->state == ENROLLED) {
    // Update enrolled.
    enrollment->date = now();
    enrollment->state = DROPPED;
    updateEnrollmentById(enrollment->id, enrollment);
    
    // Update student.
    student->takenCourses -= 1;
    student->takenCredits -= course->credit;
    updateStudentById(student->id, student);

    printf("\nStudent with number %s dropped the course with code %s.\n", student->number, course->code);

    freeEnrollment(enrollment);
  }

  freeStudent(student);
  freeCourse(course);

  pressEnterToSeeMenu();
  return 1;
}

/**
 * Course operations.
 */
Course* newCourse() {
  Course* course = (Course*) malloc(sizeof(Course));
  if (course == NULL) {
    printf("Error: Cannot allocate memory for a course.\n");
    return NULL;
  }
  return course;
}

void* columnsToCourse(Columns* columns) {
  Course* course = newCourse();
  if (course == NULL) return NULL;

  course->id = atoi(columns->array[0]);
  course->lecturerId = atoi(columns->array[1]);
  course->code = copystr(columns->array[2]);
  course->name = copystr(columns->array[3]);
  course->semester = atoi(columns->array[4]);
  course->credit = atoi(columns->array[5]);
  course->quota = atoi(columns->array[6]);

  return course;
}

char* courseToLine(void* data) {
  Course* course = data;
  char* line = mallocstr(LINESIZE);
  sprintf(
    line,
    "%d%s%d%s%s%s%s%s%d%s%d%s%d\n",
    course->id, COLUMN_DELIMITER,
    course->lecturerId, COLUMN_DELIMITER,
    course->code, COLUMN_DELIMITER,
    course->name, COLUMN_DELIMITER,
    course->semester, COLUMN_DELIMITER,
    course->credit, COLUMN_DELIMITER,
    course->quota
  );

  return line;
}

void freeCourse(void* data) {
  Course* course = data;
  free(course->code);
  free(course->name);
  free(course);
}

Course* findCourseByColumn(int column, char* match) {
  return findCastRecordByColumn(FILE_COURSES, column, match, columnsToCourse);
}

Course* findCourseByIntColumn(int column, int match) {
  return findCastRecordByIntColumn(FILE_COURSES, column, match, columnsToCourse);
}

Course* findCourseById(int id) {
  return findCourseByIntColumn(0, id);
}

Course* findCourseByCode(char* code) {
  return findCourseByColumn(2, code);
}

IdList* findCoursesByLecturerId(int lecturerId) {
  return findAllRecordsByIntColumn(FILE_COURSES, 1, lecturerId);
}

void updateCourseById(int id, Course* course) {
  updateCastRecordByIntColumn(FILE_COURSES, 0, id, course, courseToLine);
}

void setCourseLecturerId(int courseId, int lecturerId) {
  Course* course = findCourseById(courseId);
  course->lecturerId = lecturerId;
  updateCourseById(courseId, course);
  freeCourse(course);
}

bool coursesHasRecord() {
  return fileHasRecord(FILE_COURSES);
}

bool isCourseExistById(int courseId) {
  return existanceTestHelper(findCourseById(courseId), freeCourse);
}

bool isCourseExistByCode(char* courseCode) {
  return existanceTestHelper(findCourseByCode(courseCode), freeCourse);
}

bool courseExistanceByIdTester(int courseId, int searchInt) {
  if (courseId == 0) return true;
  bool exist = isCourseExistById(courseId);
  if (!exist) {
    printf(INDENT"Error: There is no course with id #%d, press enter to skip.\n", courseId);
  }
  return exist;
}

bool courseExistanceByCodeTester(char* courseCode, char* searchStr) {
  if (strcmp(courseCode, "") == 0) return true;
  bool exist = isCourseExistByCode(courseCode);
  if (!exist) {
    printf(INDENT"Error: There is no course with code %s, press enter to skip.\n", courseCode);
  }
  return exist;
}

void courseLister(Columns* columns, bool head) {
  Course* course = columnsToCourse(columns);
  
  if (head) {
    printSeparator(141);
    printf(
      "%-5s %-15s %-30s %-40s %-15s %-15s %-15s\n",
      "id", "code", "name", "lecturer", "credit", "quota", "semester"
    );
    printSeparator(141);
  }

  char* lecturerInfo = mallocstr(40);
  if (course->lecturerId != 0) {
    Lecturer* lecturer = findLecturerById(course->lecturerId);
    lecturerInfo = buildFullLecturerInfo(lecturer);
    freeLecturer(lecturer);
  } else {
    strcpy(lecturerInfo, "n/a");
  }

  printf(
    "%-5d %-15s %-30s %-40s %-15d %-15d %-15d\n",
    course->id, course->code, course->name, lecturerInfo,
    course->credit, course->quota, course->semester
  );

  freeCourse(course);
  free(lecturerInfo);
} 

int listCourses() {
  listRecords(FILE_COURSES, courseLister, "", predicateColumnsAll);
  return 1;
}

int listCoursesOfLecturer() {
  if (!lecturersHasRecord()) {
    printf("\nThere are no lecturers available.\n");
    pressEnterToSeeMenu();
    return 1;
  }

  printf("\nEnter lecturer id to list given courses.\n");
  int lecturerId = promptIntWith("Lecturer id: ", LINESIZE, 0, 0, lecturerExistanceByIdTester);
  if (lecturerId == 0) return 1;

  printf("\n");

  char* strLecturerId = toStr(lecturerId);
  char* searchStr = buildSearchStr(1, strLecturerId);
  listRecords(FILE_COURSES, courseLister, searchStr, columnEqualityTester);

  free(strLecturerId);
  free(searchStr);
  return 1;
}

bool courseCodeUniqTester(char* code, char* currentCode) {
  if (strcmp(code, currentCode) == 0) return true;
  Course* course = findCourseByCode(code);

  if (course != NULL) {
    printf(INDENT"Error: Course exist with code %s, please enter different one.\n", code);
    freeCourse(course);
    return false;
  }

  return true;
}

int getCourseLecturer(int lecturerId) {
  if (!lecturersHasRecord()) {
    printf("\nThere are no lecturers available.\n");
    printf("You can assign lecturer later.\n");
    return 0;
  }

  printf("\n"INDENT"Lecturer Assignment\n");
  printf(INDENT"Enter a lecturer id to assign course a lecturer.\n");

  if (lecturerId) {
    printf(INDENT"* Press enter to keep same lecturer.\n");
    printf(INDENT"* Enter one space to deassign lecturer.\n");
    Lecturer* lecturer = findLecturerById(lecturerId);
    char* lecturerInfo = buildFullLecturerInfo(lecturer);
    printf("\n"INDENT"Current lecturer is %s", lecturerInfo);
    free(lecturerInfo);
    freeLecturer(lecturer);
  } else {
    printf(INDENT"Press enter to skip.\n");
  }

  return promptIntWith("\n"INDENT"Lecturer id: ", LINESIZE, lecturerId, 0, lecturerExistanceByIdTester);
}

int addCourse() {
  Course* course = newCourse();
  if (course == NULL) return 0;

  printPromptHeader("New Course", 0);

  course->code = promptStrWith("(1/5) Code: ", 15, "", "", courseCodeUniqTester);
  course->name = promptStr("(2/5) Name: ", 30, "");
  course->semester = promptInt("(3/5) Semester: ", 2, 1);
  course->credit = promptInt("(4/5) Credit: ", 2, 0);
  course->quota = promptInt("(5/5) Quota: ", 5, 0);
  course->id = getMaxId(FILE_COURSES) + 1;
  course->lecturerId = getCourseLecturer(0);

  bool inserted = insertRecord(FILE_COURSES, course, courseToLine);
  if (inserted) {
    printf(
      "\nCourse (#%d) %s %s added.\n",
      course->id,
      course->code,
      course->name
    );
  }

  freeCourse(course);
  pressEnterToSeeMenu();
  return 1;
}

int editCourse() {
  printf("\nEnter course id to edit (press enter to cancel): ");

  int courseId = promptIntWith("\n"INDENT"Course id: ", LINESIZE, 0, 0, courseExistanceByIdTester);
  if (courseId == 0) return 1;
  Course* course = findCourseById(courseId);

  printPromptHeader("Edit Course", 0);

  course->code = promptStrWith("(1/5) Code (%s): ", 15, course->code, course->code, courseCodeUniqTester);
  course->name = promptStr("(2/5) Name (%s): ", 30, course->name);
  course->semester = promptInt("(3/5) Semester (%s): ", 2, course->semester);
  course->credit = promptInt("(4/5) Credit (%s): ", 2, course->credit);
  course->quota = promptInt("(5/5) Quota (%s): ", 5, course->quota);
  course->lecturerId = getCourseLecturer(course->lecturerId);

  updateCourseById(courseId, course);
  printf("\nCourse #%d updated.\n", courseId);
  
  freeCourse(course);
  pressEnterToSeeMenu();
  return 1;
}

int deleteCourse() {
  printf("\nEnter course id to delete (press enter to cancel): ");

  int courseId = promptIntWith("\n"INDENT"Course id: ", LINESIZE, 0, 0, courseExistanceByIdTester);
  if (courseId == 0) return 1;
  Course* course = findCourseById(courseId);

  deleteEnrollmentsByCourseId(courseId);
  updateCourseById(courseId, NULL);
  printf("\nCourse #%d deleted.\n", courseId);

  freeCourse(course);  
  pressEnterToSeeMenu();
  return 1;
}

/**
 * Program setup.
 */
void purge() {
  remove(FILE_CONFIG);
  remove(FILE_STUDENTS);
  remove(FILE_COURSES);
  remove(FILE_LECTURERS);
  remove(FILE_ENROLLMENTS);
}

void create() {
  if (!createFile(FILE_CONFIG)) return;
  if (!createFile(FILE_STUDENTS)) return;
  if (!createFile(FILE_COURSES)) return;
  if (!createFile(FILE_LECTURERS)) return;
  if (!createFile(FILE_ENROLLMENTS)) return;
}

void setup() {
  remove(FILE_CONFIG);
  if (!createFile(FILE_CONFIG)) return;

  printf("---------------------------------\n");
  printf(INDENT"Setup\n");
  
  FILE* configFile = fopen(FILE_CONFIG, "a+");

  printf("(1/2) Enter maximum number of courses a student can attend: ");
  fprintf(configFile, "%s%s%d\n", CKEY_MAX_COURSES, COLUMN_DELIMITER, scanInt());

  printf("(2/2) Enter maximum number of credit a student can take: ");
  fprintf(configFile, "%s%s%d\n", CKEY_MAX_CREDITS, COLUMN_DELIMITER, scanInt());

  fprintf(configFile, "%s%s%d\n", CKEY_SETUP, COLUMN_DELIMITER, 1);

  fclose(configFile);
  printf("\nSetup completed, have fun.\n");
}

void settingsLister(Columns* columns, bool head) {
  if (head) {
    printSeparator(46);
    printf("%-30s %-15s\n", "key", "value");
    printSeparator(46);
  }

  printf(
    "%-30s %-15s\n",
    columns->array[0],
    columns->array[1]
  );
} 

int listSettings() {
  listRecords(FILE_CONFIG, settingsLister, "", predicateColumnsAll);
  return 1;
}

/**
 * Menu operations
 */
int selectMenu(char* menutext) {
  printf("%s", menutext);
  return scanInt();
}

int menuSettings() {
  int action = selectMenu(
    "---------------------------------\n"
    "    Settings Menu\n"
    "    Press enter to go main menu.\n"
    "(1) List settings\n"
    "(2) Run setup again to change configs\n"
    "(3) Purge database and reset config\n"
    "(4) Purge database and exit\n"
    "Select an action: "
  );

  switch (action) {
    case 0: return 0;
    case 1:
      listSettings();
      break;
    case 2:
      setup();
      pressEnterToSeeMenu();
      break;
    case 3:
      purge();
      create();
      setup();
      pressEnterToSeeMenu();
      break;
    case 4:
      purge();
      exit(0);
      return 0;
    default:
      printf("\nUnknown action selected!\n");
      pressEnterToSeeMenu();
      break;
  }

  return 1;
}

int menuLecturer() {
  int action = selectMenu(
    "---------------------------------\n"
    "    Lecturer Menu\n"
    "    Press enter to go main menu.\n"
    "(1) List all lecturers\n"
    "(2) Add a new lecturer\n"
    "(3) Edit a lecturer\n"
    "(4) Delete a lecturer\n"
    "Select an action: "
  );

  switch (action) {
    case 0: return 0;
    case 1: return listLecturers();
    case 2: return addLecturer();
    case 3: return editLecturer();
    case 4: return deleteLecturer();
    default:
      printf("\nUnknown action selected!\n");
      pressEnterToSeeMenu();
      break;
  }

  return 1;
}

int menuStudent() {
  int action = selectMenu(
    "---------------------------------\n"
    "    Student Menu\n"
    "    Press enter to go main menu.\n"
    "(1) List all students\n"
    "(2) Add a new student\n"
    "(3) Edit a student\n"
    "(4) Delete a student\n"
    "Select an action: "
  );

  switch (action) {
    case 0: return 0;
    case 1: return listStudents();
    case 2: return addStudent();
    case 3: return editStudent();
    case 4: return deleteStudent();
    default:
      printf("\nUnknown action selected!\n");
      pressEnterToSeeMenu();
      break;
  }

  return 1;
}

int menuCourse() {
  int action = selectMenu(
    "---------------------------------\n"
    "    Course Menu\n"
    "    Press enter to go main menu.\n"
    "(1) List all courses\n"
    "(2) Add a new course\n"
    "(3) Edit a course\n"
    "(4) Delete a course\n"
    "Select an action: "
  );

  switch (action) {
    case 0: return 0;
    case 1: return listCourses();
    case 2: return addCourse();
    case 3: return editCourse();
    case 4: return deleteCourse();
    default:
      printf("\nUnknown action selected!\n");
      pressEnterToSeeMenu();
      break;
  }

  return 1;
}

int menuEnrollment() {
  int action = selectMenu(
    "---------------------------------\n"
    "    Enrollment Menu\n"
    "    Press enter to go main menu.\n"
    "(1) List all enrollments\n"
    "(2) List enrolled courses of a student\n"
    "(3) List enrolled students of a course\n"
    "(4) Export enrolled students of a course\n"
    "(5) Enroll student in a course\n"
    "(6) Drop course enrollment of a student\n"
    "Select an action: "
  );

  switch (action) {
    case 0: return 0;
    case 1: return listAllEnrollments();
    case 2: return listEnrolledCoursesOfStudent();
    case 3: return listEnrolledStudentsOfCourse();
    case 4: return exportEnrolledStudentsOfCourse();
    case 5: return enrollStudent();
    case 6: return dropEnrolledCourse();
    default:
      printf("\nUnknown action selected!\n");
      pressEnterToSeeMenu();
      break;
  }

  return 1;
}

int menuTeaching() {
  int action = selectMenu(
    "---------------------------------\n"
    "    Teaching Menu\n"
    "    Press enter to go main menu.\n"
    "(1) List courses given by a lecturer\n"
    "Select an action: "
  );

  switch (action) {
    case 0: return 0;
    case 1: return listCoursesOfLecturer();
    default:
      printf("\nUnknown action selected!\n");
      pressEnterToSeeMenu();
      break;
  }

  return 1;
}

int loopSubmenu(Menu submenu) {
  while(submenu());
  return 1;
}

int menu() {
  int submenu = selectMenu(
    "---------------------------------\n"
    "    Main Menu\n"
    "    Press enter to exit.\n"
    "(1) Settings\n"
    "(2) Lecturer Operations\n"
    "(3) Course Operations\n"
    "(4) Student Operations\n"
    "(5) Enrollment Operations\n"
    "(6) Teaching Operations\n"
    "Select a submenu: "
  );

  switch (submenu) {
    case 0: return 0;
    case 1: return loopSubmenu(menuSettings);
    case 2: return loopSubmenu(menuLecturer);
    case 3: return loopSubmenu(menuCourse);
    case 4: return loopSubmenu(menuStudent);
    case 5: return loopSubmenu(menuEnrollment);
    case 6: return loopSubmenu(menuTeaching);
    default:
      printf("\nUnknown submenu selected!\n");
      pressEnterToSeeMenu();
      break;
  }

  return 1;
}

/**
 * Program entry.
 */
int main() {
  // Create database and run setup wizard if this is initial run.
  if (readConfig(CKEY_SETUP) == -1) {
    printf("Welcome to Course Automation YTU2020!\n");
    printf("This is the first time run of this program.\n");
    printf("So you need to do the initial setup.\n\n");
    create();
    setup();
    pressEnterToSeeMenu();
  }

  // Main loop.
  while (menu());
  return 0;
}
