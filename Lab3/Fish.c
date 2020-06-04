/*
    Alejandro Martin
    CECS326
    Lab 3
*/

//libraries to include in program
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

// Sets river width to 11
#define RIVER_WIDTH 11
// Sets river width to 11
#define RIVER_HEIGHT 11
// Constant 30 second timer
#define TIME 30

//constant river size is equal to 11 x 11 = 121
static const int RIVER_SIZE = 121;

// Semaphore declaration of swim_mill
sem_t *mill_sem;
// Semaphore declaration of fish
sem_t *fish_sem;
// Semaphore declaration of pellet
sem_t *pellet_sem;

// Method instances
int findPellet();
void moveLeft();
void moveRight();
void stay();

// Points to start of shared memory
char *shm, *fish;
// Column of where the fish is
int col;

// Main Function
int main(){
	// Sets up and identifies shared memory
	int shmid;
	key_t shm_key = 1234;

	// Locates shared memory segment
	if((shmid = shmget(shm_key, RIVER_SIZE, 0666)) < 0)
    {
		perror("shmget");
		exit(1);
	}

	// Attachs shared memory to address space of the calling process
	if((shm = shmat(shmid, NULL, 0)) == (char *) -1)
    {
		perror("shmat");
		exit(1);
	}

    // Opens fish and pellet semaphores
	fish_sem = sem_open("/fish_sem", 0);
	pellet_sem = sem_open("/pellet_sem", 0);
	// Sets up fish location
	fish = shm + RIVER_HEIGHT * (RIVER_WIDTH - 1) + RIVER_WIDTH / 2;
    // Puts 'F' character on river bank as the "fish"
	*fish = 'F';
    // Sets fish to middle of river bank
	col = RIVER_WIDTH / 2;

	// Finds pellet and moves fish towards it
	while(1){
		// Get direction of closest reachable pellet
		int dir = findPellet();
		// Locks pellet threads
		sem_wait(fish_sem);
		// Fish moves in direction of pellet
		if(dir < 0)
        {
            // Goes to moveRight() function
			moveRight();
		} else if(dir > 0)
        {
            // Goes to moveLeft() function
			moveLeft();
		} else
        {
            // Goes to stay() function
			stay();
		}
		// Unlocks pellet_sem so it can run
		sem_post(pellet_sem); 
	}
    // Exits main function
	return(0);
}

// Function that moves the fish to the right.
void moveRight()
{
    // Outputs either a '~' or 'F' character on river bank
	if (col != RIVER_WIDTH - 1)
    {
        // Outputs river character
		*fish = '~';
        // Increments fish location
		fish++;
        // Outputs fish character
		*fish = 'F';
        // Increments column of fish location
		col++;
	}
}

// Moves the fish left. If it can't move further left, does nothing.
void moveLeft()
{
    // Outputs either a '~' or 'F' character on river bank
	if (col != 0)
    {
        // Outputs river character
		*fish = '~';
        // Decrements fish location
		fish--;
        // Outputs fish character
		*fish = 'F';
        // Decrements column of fish location
		col--;
	}
}

// Fish does not move and stays where it is at and outputs fish character
void stay()
{
    // Outputs fish character
	*fish = 'F';
}

// Function helps fish find nearest pellet to itself
int findPellet()
{
	// Width of the fishes search
	int width = 1;

	// Iterates through river rows, starting with the row right about fish, then moves upwards
	for(int i = 0; i < (RIVER_HEIGHT - 1); i++)
    {
		// Left and right pointers start in same column as fish
		char *left = fish - RIVER_WIDTH * i;
		char *right = fish - RIVER_WIDTH * i;

		// If fish is in same column as pellet, then fish does not move
		if(*left == 'o')
        {
			return(0);
		}

		// Moves fish left and right and makes sure fish does not exceed river bank
		for(int j = 0; j < width; j++)
        {
			// Makes sure fish does not exceed too far left off river bank
			if((left - shm) % RIVER_WIDTH != 0)
            {
                // Increment fish movement to left
				left--;
				if(*left == 'o')
                {
					return(1);
				}
			}

			// Makes sure fish does not exceed too far left off river bank
			if((right - shm) % RIVER_WIDTH != 10)
            {
                // Increment fish movement to the right
				right++;
				if(*right == 'o')
                {
					return(-1);
				}
			}
		}
        // Increments width search by 1
		width++;
        // Checks to make sure river width does not get larger than 11
		if(width > RIVER_WIDTH/2)
			width = RIVER_WIDTH/2;
	}
	// Fish moves toward center of river if no pellet is found
	return(col - 5);
}
