#include "stdio.h"
#include "stdlib.h"
#include "string.h"

typedef struct Student {
  int id;
  char name[20];
  int age;
  int score;
} Student;

void nota_gore_sirala(Student** students, int n) {
  int i, j;
  for (i = 0; i < n-1; i++)  {
    for (j = 0; j < n-i-1; j++)  {
      Student* studentA = *(students + j);
      Student* studentB = *(students + j + 1);

      if (studentA->score < studentB->score) {
        Student temp = *studentA;
        *studentA = *studentB;
        *studentB = temp;
      }
    }
  } 
}

void ada_gore_sirala(Student** students, int n) {
  int i, j;
  for (i = 0; i < n-1; i++)  {
    for (j = 0; j < n-i-1; j++)  {
      Student* studentA = *(students + j);
      Student* studentB = *(students + j + 1);

      if (strcmp(studentA->name, studentB->name) > 0) {
        Student temp = *studentA;
        *studentA = *studentB;
        *studentB = temp;
      }
    }
  }  
}

void liste_yaz(Student** students, int n) {
  int i;
  for(i=0; i < n; i++) {
    Student* student = *(students + i);
    printf("%d. Ogrenci: %d - %s - %d - %d \n", i + 1, student->id, student->name, student->age, student->score);
  }   
}

int main() {
  int n;

  printf("Toplam ogrenci sayisini giriniz: ");
  scanf("%d", &n);

  Student** students = (Student**) malloc(n * sizeof(Student *));

  int i;
  for(i=0; i < n; i++) {
    students[i] = (Student*) malloc(sizeof(Student));
    Student* student = students[i];

    printf("%d. ogrencinin adi: ", i + 1);
    scanf("%s", student->name);
    printf("%d. ogrencinin yasi: ", i + 1);
    scanf("%d", &student->age);
    printf("%d. ogrencinin ortalamasi: ", i + 1);
    scanf("%d", &student->score);
    student->id = i;
  }

  printf("Girmis oldugunuz ogrencilerin ilk hali: \n");
  liste_yaz(students, n);

  nota_gore_sirala(students, n);
  printf("Girmis oldugunuz ogrencilerin nota gore siralanmis hali: \n");
  liste_yaz(students, n);

  ada_gore_sirala(students, n);
  printf("Girmis oldugunuz ogrencilerin son hali: \n");
  liste_yaz(students, n);

  free(students);

  return 0;
}
