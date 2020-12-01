#include "stdio.h"
#include "stdlib.h"
#include "string.h"

typedef struct Room {
  int id;
  int width;
  int height;
  int length;
} Room;

void GetInfo(Room* rooms, int n) {
  int i;
  for(i=0; i < n; i++) {
    printf("Please enter details of room #%d\n", i);
    rooms[i].id = i;
    
    printf("Width: ");
    scanf("%d", &rooms[i].width);
    printf("Height: ");
    scanf("%d", &rooms[i].height);
    printf("Length: ");
    scanf("%d", &rooms[i].length);
  }
}

void ScanRooms(Room* rooms, int n) {
  int i;
  for(i=1; i < n-1; i++) {
    Room prevRoom = rooms[i-1];
    Room currRoom = rooms[i];
    Room nextRoom = rooms[i+1];
    int prevVolume = prevRoom.width * prevRoom.height * prevRoom.length;
    int currVolume = currRoom.width * currRoom.height * currRoom.length;
    int nextVolume = nextRoom.width * nextRoom.height * nextRoom.length;

    if (currVolume > prevVolume && currVolume > nextVolume) {
      printf(
        "Room #%d has bigger volume than room #%d and room #%d.\n",
        currRoom.id, prevRoom.id, nextRoom.id
      );
    }
  }
}

void sortbyVolume(Room* rooms, int n) {
  int i, j;
  for (i = 0; i < n-1; i++)  {
    for (j = 0; j < n-i-1; j++)  {
      Room roomA = rooms[j];
      Room roomB = rooms[j+1];
      int roomAVolume = roomA.width * roomA.height * roomA.length;
      int roomBVolume = roomB.width * roomB.height * roomB.length;

      if (roomAVolume > roomBVolume) {
        Room temp = roomA;
        rooms[j] = roomB;
        rooms[j+1] = temp;
      }
    }
  }
}

void printInfo(Room* rooms, int n) {
  int i;
  for(i=0; i < n; i++) {
    int volume = rooms[i].width * rooms[i].height * rooms[i].length;
    printf(
      "Room id=%d, width=%d, height=%d, length=%d, volume=%d \n",
      rooms[i].id, rooms[i].width, rooms[i].height, rooms[i].length, volume
    );
  }
}

int main() {
  int n;

  printf("Enter the total room count: ");
  scanf("%d", &n);

  Room* rooms = (Room*) malloc(n * sizeof(Room));

  printf("---GetInfo\n");
  GetInfo(rooms, n);
  printf("---ScanRooms\n");
  ScanRooms(rooms, n);
  printf("---PrintInfo\n");
  printInfo(rooms, n);
  printf("---SortByVolume\n");
  sortbyVolume(rooms, n);
  printInfo(rooms, n);

  free(rooms);

  // Please remove line to make program stop before exit.
  // system("PAUSE");
  return 0;
}
