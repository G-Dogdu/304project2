#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

int randomNumber(int lower_bound, int upper_bound);
int seed = 10;

int main() {

  //   printf("start");
  srand(seed);

  for (int i = 0; i < 100; i++) {
    usleep(1000);

    int new = randomNumber(0, 100);
    // printf("new value is:%d\n", new);
    if (new <= 90) {

      printf("new job: %d\n", new);
    } else if (new > 90) {
      printf("no job: %d\n", new);
    }
  }

  return 0;
}

int randomNumber(int lower_bound, int upper_bound) {
  return (lower_bound + rand()) /
         (RAND_MAX / (upper_bound - lower_bound + 1) + 1);
}
