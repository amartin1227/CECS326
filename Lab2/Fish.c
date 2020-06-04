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

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHM_SIZE 60

int main(){

  // global variables
  // shared memory array denoting location
  int *shmem;
  int shmid;
  // declares key
  int key = 5678;
  // contains shmdt output
  int shmdtOut;

  int i;
  // holds closest pellet to fish
  int closest;
  int fishColumn;

  // pellet column
  int pelColumn;
  // pellet row
  int pelRow;
  // distance of pellet from fish
  int pelDistance;
  
  // closest pellet column
  int closestColumn;
  // closest pellet row
  int closestRow;
  // closest distance of pellet from fish
  int closestDist;

  // allocates shared memory segment
  if( (shmid = shmget(key, SHM_SIZE, 0666)) < 0 ){
    perror("shmget");
    exit(1);
  }

  // attach shared memory segment to address space
  if( (shmem = shmat(shmid, NULL, 0)) == (int *) -1){
    perror("shmat");
    exit(1);
  }

  // change shared memory to denote fish's position to bottom row
  // bottom row is in range between 90-99
  shmem[0] = 94;
  closest = 1;  

  // fish keeps running
  while( shmem[12] < shmem[13] ){
    fishColumn = shmem[0]%10;

    // scan shmem and direct fish to column of closest pellet
    for( i = 1; i <= shmem[11]; i++ ){
      pelColumn = shmem[i]%10;
      pelRow = (shmem[i]/10)%10;
      closestColumn = shmem[closest]%10;
      closestRow = (shmem[closest]/10)%10;
        
      // closest pellet is biggest row number by default
      if( (pelRow > closestRow) && (pelRow != 9) ){
        closest = i;
        closestColumn = pelColumn;
      }

      // if pellets happen to be in the same row number
      // then :
      //    a) closest column number relative to fish
      //    - smallest abs(fish column - pellet column)
      //    b) if 2 equally distant pellets, leftmost/smaller column chosen
      else if( pelRow == closestRow ){
        pelDistance = abs(fishColumn-pelColumn);
        closestDist = abs(fishColumn-closestColumn);

        // a) if current pellet is closer than closest pellet
        // closest becomes current pellet
        if( pelDistance < closestDist ){
          closest = i;
          closestColumn = pelColumn;
        }
        
        // b) if current and closest pellet are equally distant
        else if( pelDistance == closestDist ){
          // closest is leftmost pellet
          if( shmem[i] < shmem[closest] ){
            closest = i;
            closestColumn = pelColumn;
          }
        }
      }
    }
    
    // move fish one unit closer to closest pellet
    if( closestColumn > fishColumn )
      shmem[0]++;
    else if( closestColumn < fishColumn )
      shmem[0]--;
    // suspend execution of thread for 2 seconds
    sleep(2);
  }
  // fish has exited
  printf("fish exited\n");
  
  // deallocate shared memory
  shmdtOut = shmdt(shmem);
  if( shmdtOut == -1 ){
    perror("shmdt");
    exit(1);
  }
  exit(0);
}

