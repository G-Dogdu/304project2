#include "queue.c"
#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

int simulationTime = 120; // simulation time
int seed = 10;            // seed for randomness
int emergencyFrequency =
    30;     // frequency of emergency gift requests from New Zealand
int id = 0; // Job id

int GS_count = 0; // Game Stop Task Count
// Functions Declarations
void *ElfA(void *arg); // the one that can paint
void *ElfB(void *arg); // the one that can assemble
void *Santa(void *arg);
void *ControlThread(void *arg); // handles printing and queues (up to you)
int randomNumber(int lower_bound, int upper_bound);
void Assignjob();
void add_GS_count(int add); // for keeping track of the GS count
// Queue for all jobs
Queue *Painting;
Queue *Assembly;
Queue *Packaging;
Queue *QA;
Queue *Delivery;
// threads declarations
pthread_t elfa;
pthread_t elfb;
pthread_t santa;
pthread_t control;
pthread_mutex_t GS_count_mutex;
// pthread sleeper function
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
  // Gb_count_mutex initializer
  pthread_mutex_init(&GS_count_mutex, NULL);

  srand(seed); // feed the seed

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
  // Queue Initializations

  Painting = ConstructQueue(100);
  Packaging = ConstructQueue(100);
  Delivery = ConstructQueue(100);
  Assembly = ConstructQueue(100);
  QA = ConstructQueue(100);
  // threads initializations
  pthread_create(&control, NULL, ControlThread, NULL);
  pthread_create(&elfa, NULL, ElfA, NULL);
  pthread_create(&elfb, NULL, ElfB, NULL);
  pthread_create(&santa, NULL, Santa, NULL);
  // wait until all threads to close
  pthread_join(control, NULL);
  pthread_join(elfa, NULL);
  pthread_join(elfb, NULL);
  pthread_join(santa, NULL);

  return 0;
}

void *ElfA(void *arg) {
  while (1) {
    // lock packaging and painting
    pthread_mutex_lock(&Packaging->lock);
    pthread_mutex_lock(&Painting->lock);
    // check for packaging
    if (!isEmpty(Packaging)) {
      // unlock painting
      pthread_mutex_unlock(&Painting->lock);
      // dequeue the job from queue
      Task Current_Job = Dequeue(Packaging);
      // unlock packaging
      pthread_mutex_unlock(&Packaging->lock);
      // do the job
      printf("Elf A packaging job with id=%d\n", Current_Job.ID);
      pthread_sleep(1);
      // set the job boolean to true
      Current_Job.packaging_done = true;
      // sent to delivery queue
      printf("Elf A packaging job done and sent to delivery with id=%d\n",
             Current_Job.ID);
      // lock delivery queue
      pthread_mutex_lock(&Delivery->lock);
      // enqueue to delivery queue
      Enqueue(Delivery, Current_Job);
      // unlock delivery queue
      pthread_mutex_unlock(&Delivery->lock);
    } else if (!isEmpty(Painting)) { // check for painting
      // unlock packaging
      pthread_mutex_unlock(&Packaging->lock);
      // dequeue the job from queue
      Task Current_Job = Dequeue(Painting);
      // unlock painting lock
      pthread_mutex_unlock(&Painting->lock);
      // do the job
      printf("Elf A painting found with id=%d!\n", Current_Job.ID);
      pthread_sleep(3);
      // set the job boolean to true
      Current_Job.painting_done = true;
      if (Current_Job.type >=
          4) { // check for job type to see if it requires QA
        add_GS_count(1);
        // do the job
        printf("Elf A sent package to QA with id=%d!\n", Current_Job.ID);
        // lock the QA
        pthread_mutex_lock(&QA->lock);
        // Enqueue to QA queue
        Enqueue(QA, Current_Job);
        // unlock the QA
        pthread_mutex_unlock(&QA->lock);
      } else { // check for job type if it is less than (type 4 or higher)
        // lock Packaging
        pthread_mutex_lock(&Packaging->lock);
        // Enqueue Packaging
        Enqueue(Packaging, Current_Job);
        // sent to packaging after painting
        printf("Elf A sent to packaging after painting\n");
        // unlock packaging
        pthread_mutex_unlock(&Packaging->lock);
      }
    } else {
      // unlock packaging
      pthread_mutex_unlock(&Packaging->lock);
      // unlock painting
      pthread_mutex_unlock(&Painting->lock);
    }
  }
}

void *ElfB(void *arg) {
  while (1) {
    // lock packaging and assembly
    pthread_mutex_lock(&Packaging->lock);
    pthread_mutex_lock(&Assembly->lock);
    if (!isEmpty(Packaging)) {
      // unlock assembly
      pthread_mutex_unlock(&Assembly->lock);
      // dequeue the job from queue
      Task Current_Job = Dequeue(Packaging);
      // unlock packaging
      pthread_mutex_unlock(&Packaging->lock);
      // do the packaging job
      printf("Elf B packaging job with id=%d\n", Current_Job.ID);
      pthread_sleep(1);
      // set the job boolean to true
      Current_Job.packaging_done = true;
      // sent to delivery queue
      printf("Elf B packaging done and sent to delivery with id=%d\n",
             Current_Job.ID);
      // lock delivery
      pthread_mutex_lock(&Delivery->lock);
      // enqueue delivery queue
      Enqueue(Delivery, Current_Job);
      // unlock delivery
      pthread_mutex_unlock(&Delivery->lock);
    } else if (!isEmpty(Assembly)) { // check for assembly
      // unlock packaging lock
      pthread_mutex_unlock(&Packaging->lock);
      // dequeue current_job from assembly queue
      Task Current_Job = Dequeue(Assembly);
      // unlock assembly lock
      pthread_mutex_unlock(&Assembly->lock);
      // do the assembly job
      printf("Elf B assembly job with id=%d!\n", Current_Job.ID);
      pthread_sleep(2);
      // set the job boolean true
      Current_Job.assembly_done = true;
      if (Current_Job.type >= 4) { // check for job type if it requires QA
        add_GS_count(1);
        // do the job
        printf("Elf B sent package to QA with id=%d!\n", Current_Job.ID);
        // lock the QA
        pthread_mutex_lock(&QA->lock);
        // Enqueue the QA queue
        Enqueue(QA, Current_Job);
        // unlock the QA
        pthread_mutex_unlock(&QA->lock);
      } else { // check for job type if it is less than (type 4 or higher)
        // lock Packaging queue
        pthread_mutex_lock(&Packaging->lock);
        // Enqueue to Packaging queue
        Enqueue(Packaging, Current_Job);
        printf("Elf B sent to packaging after assembly\n");
        // unlock Packaging
        pthread_mutex_unlock(&Packaging->lock);
      }
    } else {
      // unlock Packaging and Assembly
      pthread_mutex_unlock(&Packaging->lock);
      pthread_mutex_unlock(&Assembly->lock);
    }
  }
}

// manages Santa's tasks
void *Santa(void *arg) {
  while (1) {
    // lock Delivery, QA and GS_count_mutex
    pthread_mutex_lock(&Delivery->lock);
    pthread_mutex_lock(&QA->lock);
    pthread_mutex_lock(&GS_count_mutex);
    if (GS_count < 3 && !isEmpty(Delivery)) { // Apply the GS_count condition
      // printf("GB count: %d\n", GS_count);
      // unlock GS_count_mutex
      pthread_mutex_unlock(&GS_count_mutex);
      // unlock QA_lock
      pthread_mutex_unlock(&QA->lock);
      // Dequeue the Current_Job
      Task Current_Job = Dequeue(Delivery);
      // Add to GS job
      if (Current_Job.type == 5 || Current_Job.type == 6) {
        add_GS_count(-1);
      }
      // unlock Delivery and do the delivery job
      pthread_mutex_unlock(&Delivery->lock);
      pthread_sleep(1);
      // set the delivery job boolean true
      Current_Job.delivery_done = true;
      printf("Santa done delivery with id=%d !\n", Current_Job.ID);
      // check for if the job done or not
      if (!(Current_Job.delivery_done && Current_Job.packaging_done)) {
        fprintf(stderr, "Issue with job id=%d type=%d\n", Current_Job.ID,
                Current_Job.type);
        exit(1);
      }
      // check for all job types to be done
      if (!(Current_Job.type == 2 ||
            (Current_Job.type == 3 && Current_Job.painting_done) ||
            (Current_Job.type == 4 && Current_Job.assembly_done) ||
            (Current_Job.type == 5 && Current_Job.painting_done &&
             Current_Job.qa_done) ||
            (Current_Job.type == 6 && Current_Job.assembly_done &&
             Current_Job.qa_done))) {
        printf("Job with id :%d\n", Current_Job.ID);
        printf("Delivery: %dexit\n", Current_Job.delivery_done);
        printf("Packaging: %d\n", Current_Job.packaging_done);
        printf("Painting: %d\n", Current_Job.packaging_done);
        printf("Assembly: %d\n", Current_Job.packaging_done);
        printf("QA: %d\n", Current_Job.qa_done);
        fprintf(stderr, "Issue2 with job id=%d type=%d\n", Current_Job.ID,
                Current_Job.type);
        exit(1);
      }
    } else if (!isEmpty(QA)) { // check for QA job
      printf("GB count: %d\n", GS_count);
      // unlock GS_count_mutex
      pthread_mutex_unlock(&GS_count_mutex);
      // unlock Delivery
      pthread_mutex_unlock(&Delivery->lock);
      // Dequeue the QA
      Task Current_Job = Dequeue(QA);
      // unlock QA and do the QA job
      pthread_mutex_unlock(&QA->lock);
      printf("Santa received QA with id=%d!\n", Current_Job.ID);
      pthread_sleep(1);
      // set the job boolean true
      Current_Job.qa_done = true;
      // lock Packaging
      pthread_mutex_lock(&Packaging->lock);
      // Enqueue to Packaging
      Enqueue(Packaging, Current_Job);
      // unlock Packaging
      pthread_mutex_unlock(&Packaging->lock);
      printf("Santa done QA and sent id=%d to packaging!\n", Current_Job.ID);
    } else {
      // Unlock Delivery, QA and GS_count_mutex
      pthread_mutex_unlock(&Delivery->lock);
      pthread_mutex_unlock(&QA->lock);
      pthread_mutex_unlock(&GS_count_mutex);
    }
  }
}

// the function that controls queues and output
void *ControlThread(void *arg) {
  struct timeval current, start;
  gettimeofday(&start, NULL);
  gettimeofday(&current, NULL);
  // run the code in simulation time
  while (current.tv_sec < start.tv_sec + simulationTime) {
    pthread_sleep(1);
    // assign job
    Assignjob();

    gettimeofday(&current, NULL);
  }
  pthread_exit(&elfa);
  pthread_exit(&elfb);
  pthread_exit(&santa);
  pthread_exit(&control);
}

void Assignjob() { // Assign job to queues, according to their type
  int rand_num = randomNumber(0, 99);
  id++; // increment the job id
  Task Job;
  // initialize Job, set the boolean values of Job to false
  memset(&Job, 0, sizeof(Job));

  if (rand_num >= 85 && rand_num < 90) {
    printf("id=%d type=%d\n", id, 6);
    // Excellent 2 chance
    Job.ID = id;
    Job.type = 6;
    Enqueue(Assembly, Job);
    printf("Kid with behaviour: excellent with id %d\n", Job.ID);

  } else if (rand_num >= 80 && rand_num < 85) {
    printf("id=%d type=%d\n", id, 5);
    // Excellent 1 chance
    Job.ID = id;
    Job.type = 5;
    Enqueue(Painting, Job);
    printf("Kid with behaviour: excellent with id %d\n", Job.ID);

  } else if (rand_num >= 60 && rand_num < 80) {
    printf("id=%d type=%d\n", id, 4);
    // Good2
    Job.ID = id;
    Job.type = 4;
    Enqueue(Assembly, Job);
    printf("Kid with behaviour: good with id %d\n", Job.ID);

  } else if (rand_num >= 40 && rand_num < 60) {
    printf("id=%d type=%d\n", id, 3);
    // Good1
    Job.ID = id;
    Job.type = 3;
    Enqueue(Painting, Job);
    printf("Kid with behaviour: good with id %d\n", Job.ID);

  } else if (rand_num >= 0 && rand_num < 40) {
    printf("id=%d type=%d\n", id, 2);
    // Okay
    Job.ID = id;
    Job.type = 2;
    Enqueue(Packaging, Job);
    printf("Kid with behaviour: okay with id %d\n", Job.ID);
  } else if(rand_num >=90 && rand_num < 100) {
    // bad kid
    Job.ID = id;
    Job.type = 2;
    Enqueue(Packaging, Job);
    printf("Kid with behaviour: bad with id %d\n", Job.ID);
  }
}
// random number generator
int randomNumber(int lower_bound, int upper_bound) {
  return ((lower_bound + rand()) /
          (RAND_MAX / (upper_bound - lower_bound + 1) + 1));
}

void add_GS_count(int add) {
  pthread_mutex_lock(&GS_count_mutex);
  GS_count += add;
  pthread_mutex_unlock(&GS_count_mutex);
}