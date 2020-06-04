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
#include <pthread.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/syscall.h>

// min number pellets
#define PELLET_MIN 5
// max number pellets
#define PELLET_MAX 10
#define SHM_SIZE 60

// global variables
int pellet;
int *shmem;
int shmid;
int key;
int shmdtOut;

void *pelletChild(void *ignored){
 
  //position variable
  int pos;
  //pellet variable
  int thisPellet = pellet;
  //thread id
  long pid = syscall(SYS_gettid);
    
  // generate random pellet location in first 5 rows
  srand(time(NULL));
  int r = rand() % 50;
  
  int i;
  // verifies pellet does not overlap with another
  for( i = 0; i < pellet; i++ ){
    while( r == shmem[i] ){
      r = rand() % 50;
    }
  }

  shmem[thisPellet] = r;
  //tracks number of pellets
  shmem[11] = thisPellet;

  // thread keeps running until cancelled
  while( shmem[12] < shmem[13] ){
    // suspends execution of thread for 2 seconds
    sleep(2);
    
    // once pellet moves out of range (0-99)
    if( shmem[thisPellet] > 99 ){
      
      // Prints pellet ID, position, and status of pellet once exited
      if( shmem[thisPellet] < 110 ){
        printf("pellet ID: %Lf\n", (long double) pid);
        printf("pellet position: %d\n", shmem[thisPellet] - 10);
        // prints if pellet was missed by fish
        printf("pellet status: MISSED\n");

      }
      // Prints pellet ID, position, and status of pellet once exited
      else{
        printf("pellet ID: %Lf\n", (long double) pid);
        printf("pellet position: %d\n", shmem[0]);
        // prints if pellet was eaten by fish
        printf("pellet status: EATEN\n");
        //prints total number of pellets caught
        printf("# pellets caught: %d\n", shmem[14]);
      }

      // Goes back to random position
      srand(time(NULL));
      for( i = 0; i < pellet; i++ ){
        while( r == shmem[i] ){
          r = rand() % 50;
        }
      }
      // pellet is positioned at generated random position
      shmem[thisPellet] = r;

    }

    // makes sure generated random position does not overlap with fish
    pos = shmem[thisPellet];
    if( (pos += 10) != shmem[0] ){  
      // pellet moves down one row
      shmem[thisPellet] += 10;
    }
    
    else {
      // moves pellet completely out of range
      shmem[thisPellet] = 110;
      //increment pelletsCaughtCount
      shmem[14]++;
    }
  }
}

int main(){
  // instantiation of thread
  pthread_t child_thread;
  // contains any error that may occur
  int code;
  //shared memory variable
  key = 5678;
  pellet = 1;

  // allocates shared memory segment
  if( (shmid = shmget(key, SHM_SIZE, 0666)) < 0 ){
    perror("shmget");
    exit(1);
  }   

  // attaches shared memory segment to address space
  if( (shmem = shmat(shmid, NULL, 0)) == (int *) -1){
    perror("shmat");
    exit(1);
  }

  // parent thread creates up to 10 child threads
  while( (pellet <= PELLET_MAX) && (shmem[12] < shmem[13])){
    code = pthread_create(&child_thread, NULL, pelletChild, NULL);
    if(code){
      fprintf(stderr, "pthread_create failed with code %d\n", code);
    }
    // suspends execution of thread for 7 seconds
    sleep(7);
    pellet++;
  }
    
  // shared memory deallocated
  shmdtOut = shmdt(shmem);
  if( shmdtOut == -1 ){
    perror("shmdt");
    exit(1);
  }
  exit(0);
}
