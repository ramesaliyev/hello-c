#include "stdio.h"

int enbuyuk_bul(int* mat, int  n) {
  int biggerValue;
  int biggerIndex;

  biggerIndex = 0;
  int firstReal = (*(mat));
  int firstImag = (*(mat+1));
  biggerValue = (firstReal*firstReal) + (firstImag*firstImag);

  int i;
  for(i=1; i < n; i++) {
    int real = *((mat+i*2)+0);
    int imag = *((mat+i*2)+1);
    int sum = (real*real) + (imag*imag);

    if (sum > biggerValue) {
      biggerIndex = i;
      biggerValue = sum;
    }
  }

  return biggerIndex;
}

void liste_yazdir(int* mat, int n) {
  int i;
  for(i=0; i < n; i++) {
    printf("%d = %d + %di \n", i, *((mat+i*2)+0), *((mat+i*2)+1));
  }
}

int main() {
  int n;

  printf("Toplam karmasik sayi adedini giriniz: ");
  scanf("%d", &n);

  int complex[n][2];
  int i;

  for(i=0; i < n; i++) {
    printf("%d. karmasik sayinin reel kismini girin: ", i + 1);
    scanf("%d", &complex[i][0]);

    printf("%d. karmasik sayinin sanal kismini girin: ", i + 1);
    scanf("%d", &complex[i][1]);
  }

  printf("Girmis oldugunuz karmasik sayilar: \n");
  liste_yazdir(complex[0], n);
  
  printf("Reel ve Sanal kisimlarinin kareleri toplami en buyuk olan sayinin yer aldigi satir: \n");
  int rown = enbuyuk_bul(complex[0], n);
  printf("%d\n", rown + 1);

  return 0;
}
