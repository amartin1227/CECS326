/*
    Alejandro Martin
    CECS326
    Lab 2
*/

// libraries
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHM_SIZE 60

static volatile int keepRunning = 1;

// terminates if dummy signal is sent
void intHandler(int dummy) {
  keepRunning = 0;
  // prints out process being killed
  printf("\nProcess killed.\n");
}
  
int main(void){

  // local variables
  // shared memory array denoting location
  int *shmem;
  int shmid;
  // declares key
  int key = 5678;
  // contains shmdt output
  int shmdtOut;
 
  // creates visible swim mill to terminal
  char swimMill[99];

  // file to where output of swim mill gets printed
  FILE *f = fopen("swim_mill output.txt", "w");

  // pids for swim mill parent
  pid_t parentId = getpid();
  // pids for fish/pellet child processes
  pid_t fishId, pelletId;

  // length of computation
  clock_t endwait;
  // start time of clock
  clock_t start = clock();
  // clock duration of process is set to 30 seconds
  clock_t duration = 30;
  // total length of computation
  endwait = start + duration;

  // allocates shared memory segment
  shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
  if( shmid < 0 ){
    perror("shmget");
    exit(1);
  }
  // attaches shared memory segment identified by shmid
  // to the address space of the calling process
  shmem = shmat(shmid, NULL, 0);
  if( shmem == (int*) -1 ){
    perror("shmat");
    exit(1);
  } 
  
  // ensures that all processes only run for 30 second duration
  shmem[12] = start;
  shmem[13] = endwait;
  
  // sets pellet caught count to 0 to start
  shmem[14] = 0;

  signal(SIGINT, intHandler);

  // creating fork child process for fish
  fishId = fork();

  // if fork child process for fish is negative value,
  // creation of child process was unsuccessful
  // and error message is printed
  if( fishId < 0 ){
    perror("cannot fork\n");
    exit(1);
  }
  
  else if( fishId > 0 ){
    // creating fork child process for pellet
    pelletId = fork();
      
    // if fork child process for pellet is negative value,
    // creation of child process was unsuccessful
    // and error message is printed
    if( pelletId < 0 ){
      perror("cannot fork\n");
      exit(1);
    }
  
    else if( pelletId > 0 ){
      // suspends execution of thread for 2 seconds
      sleep(2);

      // computation time begins
      while( (start < endwait) && keepRunning ){

        // fills in or clears swim mill
        int i;
        // fills swim mill with ~ indicating the river
        for( i = 0; i < 100; i++ ){
          swimMill[i] = '~';
        }

        // assign the pellets location based on shared memory
        for( i = 1; i <= shmem[11]; i++ ){
          swimMill[shmem[i]] = 'o';
        }

        // assign the fish location based on shared memory
        swimMill[shmem[0]] = 'F';

        // print swim mill && write result to file
        for( i = 0; i < 100; i++ ){  
          
          if( i%10 == 0 ){
            printf("\n%c", swimMill[i]);
            fprintf(f, "\n%c", swimMill[i]);
          }
  
          else{
            printf("%c", swimMill[i]);
            fprintf(f, "%c", swimMill[i]);
          }

          if(i == 99){
            printf("\n\n");
            fprintf(f, "\n\n");
          }
        }
        // suspends execution of thread for 2 seconds
        sleep(2);
        start++;
        shmem[12] = start;   
      }
    }
    else{
      // executes and leaves pellet process
      int pelletOut = execl("./pellet", "./pellet", NULL); 
      if( pelletOut == -1 ){
        perror("execl pellet");
        exit(1);
      }
      _exit(EXIT_FAILURE);
    }
  }
  else{

    // executes and leaves fish process
    int fishOut = execl("./fish", "./fish", NULL);
    if( fishOut == -1 ){
      perror("execl fish");
      exit(1);
    }
    _exit(EXIT_FAILURE);
  }
  // closes output file
  fclose(f);

  // all children of swim_mill killed
  // kills fish process
  kill(fishId, SIGKILL);
  printf("fish process exited\n");
  // kills pellet process
  kill(pelletId, SIGKILL);
  printf("pellet process exited\n");

  printf("swim mill exited\n");

  shmdtOut = shmdt(shmem);
  if( shmdtOut == -1 ){
    perror("shmdt");
    exit(1);
  }
  exit(0);
}
