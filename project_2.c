#include "queue.c"
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

int simulationTime = 120; // simulation time
int seed = 10;            // seed for randomness
int emergencyFrequency =
    30; // frequency of emergency gift requests from New Zealand

void *ElfA(void *arg); // the one that can paint
void *ElfB(void *arg); // the one that can assemble
void *Santa(void *arg);
void *ControlThread(void *arg); // handles printing and queues (up to you)
int randomNumber(int lower_bound, int upper_bound);
// pthread sleeper function
int ID = 0;
struct Kid {
  char *behaviour;
  int id;
  int present;

} typedef Kid;

char *jobs[4] = {"Chocolate", "Wooden Toy", "Plastic Toy", "GS5"};
struct job {
  int j_id;
  bool is_ready;
} typedef Job;

struct Present {
  char *jobs;
  int p_id;
  int size;
} typedef Present;

int pthread_sleep(int seconds) {
  pthread_mutex_t mutex;
  pthread_cond_t conditionvar;
  struct timespec timetoexpire;
  if (pthread_mutex_init(&mutex, NULL)) {
    return -1;
  }
  if (pthread_cond_init(&conditionvar, NULL)) {
    return -1;
  }
  struct timeval tp;
  // When to expire is an absolute time, so get the current time and add it to
  // our delay time
  gettimeofday(&tp, NULL);
  timetoexpire.tv_sec = tp.tv_sec + seconds;
  timetoexpire.tv_nsec = tp.tv_usec * 1000;

  pthread_mutex_lock(&mutex);
  int res = pthread_cond_timedwait(&conditionvar, &mutex, &timetoexpire);
  pthread_mutex_unlock(&mutex);
  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&conditionvar);

  // Upon successful completion, a value of zero shall be returned
  return res;
}

int main(int argc, char **argv) {
  // -t (int) => simulation time in seconds
  // -s (int) => change the random seed
  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-t")) {
      simulationTime = atoi(argv[++i]);
    } else if (!strcmp(argv[i], "-s")) {
      seed = atoi(argv[++i]);
    }
  }

  srand(seed); // feed the seed
  printf("%d\n", randomNumber(0, 100));
    int present[4] = {0, 1, 2, 3};

  Kid k1;
  char * behaviour[4] = {"Bad", "Okay", "Good", "Excellent"};

  for (int i = 0; i < 100; i++) {
    // usleep(1000);
    int new = randomNumber(0, 100);
    if (new <= 90) {
      printf("new job");
    } else if (new > 90) {
      printf("no job");
    }
  }

  /* Queue usage example
      Queue *myQ = ConstructQueue(1000);
      Task t;
      t.ID = myID;
      t.type = 2;
      Enqueue(myQ, t);
      Task ret = Dequeue(myQ);
      DestructQueue(myQ);
  */

  // your code goes here
  // you can simulate gift request creation in here,
  // but make sure to launch the threads first

  return 0;
}

void *ElfA(void *arg) {
  // paint and packaging
}

void *ElfB(void *arg) {
  // assembly and packaging
}

// manages Santa's tasks
void *Santa(void *arg) {
  // qa and delivery but priority: delivery>qa
}

// the function that controls queues and output
void *ControlThread(void *arg) {}

int randomNumber(int lower_bound, int upper_bound) {
  return ((lower_bound + rand()) /
          (RAND_MAX / (upper_bound - lower_bound + 1) + 1));
}
