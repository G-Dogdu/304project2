Project 2 Report Ali AVCI 0072779
Görkem Doğdu 0064285

Part1:
In this part, boolean values were added to the Task struct for each type of job such as Delivery, Packaging. int randomNumber(int lower_bound, int upper_bound) function was
defined and using this function random chance values were generated. Queues for all job types were defined and initialized in the main function.
void Assignjob() function was defined, and in this function all jobs were distributed according to the random number. All jobs were added to the job Queues according to the their
job types. In the ControlThread function, for 120 seconds and in every 1 second, the job was asssigned. All jobs queues were initialized with mutexes. For elf A and elf B function that run
in different threads, lock and unlock corresponding mutexes to access the data to achieve mutually exclusivity. In these functions, Dequeue function was used for pop a task from the queue.
Pthread_sleep functions were used to show that the jobs were done in specified time. After the job was done, the corresponding boolean job flag was set to true.
In ElfA and ElfB function, since the prioritized job is packaging, we always check the packaging queue first. The mutex locks were tried to kept as short as possible to avoid data races. 
In Santa function, we always check if the job is deliver or not, first. We also ensure that all jobs are done in this function. 
Part2:
We didn't encounter any race conditions in Part1. We implemented the ControlThread function in a way that all bad kids received type 2 gifts. In ElfA and ElfB function, we checked for job type, if it is higher than type 4.
Then we incremented the GS_count variable in Elf functions. If GS_count variable is higher than 3, we prioritized the QA job and we tried to avoid starvation of QA job.


