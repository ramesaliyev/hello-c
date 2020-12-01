#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "math.h"

int* withdraw_money(int amount) {
  int banknotes[] = {200, 100, 50, 20, 10, 5, 1};

  int size = 3;
  int index = 1;
  int i = 0;

  int* withdraw = (int *) malloc(sizeof(int));
  *(withdraw) = 0;

  while (amount > 0) {
    withdraw = (int *) realloc(withdraw, size * sizeof(int));
    
    int banknote = banknotes[i];
    if (amount >= banknote) {
      *(withdraw) += 1;
      *(withdraw + 1 + index) = banknote;
      *(withdraw + 1 + index + 1) = amount / banknote;
      amount = amount % banknote;

      index += 2;
      size += 2;  
    }

    i++;
  }

  return withdraw;
}

int main() {
  int money = 13; // 386

  printf("Withdrawing %d$\n", money);
  int* withdraw = withdraw_money(money);
  int n = *(withdraw);
  int i = 0;
  for (i = 1; i <= n*2; i+=2) {
    printf("Banknote: %d, Count:%d \n", *(withdraw + 1 + i), *(withdraw + 1 + i + 1));
  }

  free(withdraw);

  return 0;
}
