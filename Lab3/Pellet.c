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

//constant is equal to 11 x 11 = 121
static const int RIVER_SIZE = 121;

// Semaphore declaration of swim_mill
sem_t *mill_sem;
// Semaphore declaration of fish
sem_t *fish_sem;
// Semaphore declaration of pellet
sem_t *pellet_sem;

// Method instances
static void *pellet_func(void *ignored);

// Sets up and identifies shared memory.
int shmid;
key_t shm_key = 1234;

// Points to start of shared memory
char *shm;

// Creates file for pellet output 
FILE *fp2;
// Counter for number of pellets eaten in program
int eaten_counter;

sem_t thread_sem;
// Initializes thread total count to 0
int thread_total = 0;
// Initializes thread counter count to 0
int thread_counter = 0;

// Main Function
int main()
{
	// Array for storing threads running pellets
	pthread_t pellet_threads[TIME];

	// Opens Pellet Output file and writes to it
    // Outputs message in file
	fp2 = fopen("Pellet Output.txt", "w");
    // Outputs message
	fprintf(fp2, "Pellet tracker: \n");

	// Gets seed for random numbers
	srand(time(NULL));

	// Set flag in shmget to perform usual functions, '0666' sets
    // access permissions of the memory segment
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

    // Opens semaphore pellet
	pellet_sem = sem_open("/pellet_sem", 0);
    // Opens semaphore for swim_mill
	mill_sem = sem_open("/mill_sem", 0);

	// Initialize semaphore
	sem_init(&thread_sem, 0, 1);

    // Generates pellets while time is less than 30 seconds
    for(int i = 0; i < TIME; i++)
    {

			// Locks pellet semaphore
			sem_wait(pellet_sem);

			// Randomly generates a pellet
			if(rand() % 2 == 0)
            {

    		    // Starts a pellet thread
				int code = pthread_create(&pellet_threads[i], NULL, pellet_func, NULL);
				// Increments thread count total by 1
				thread_total++;
				// Error message is outputed is thread creation has failed and ends program
				if (code)
                {
                    // Outputs message
					fprintf(stderr, "pthread_create failed with code %d\n", code);
                    // Ends Program
					return 0;
				}
            }
            // Thread counter goes to 0
			thread_counter = 0;
            // Unlocks semaphore
			sem_post(&thread_sem);
            // Runs as long as thread counter is less than total amount of threads
			while(thread_counter < thread_total);
            // Locks pellet threads
			sem_wait(&thread_sem);
            // Unlocks swim_mill semaphore
			sem_post(mill_sem);
	}
    // Closes Pellet Output file
    fclose(fp2);
    // Exits main function
	return(0);
}

// Pellet functionality function. Checks to see if pellet is eaten or not by fish
// and writes to Pellet Output.txt file the output of the pellet information(if eaten, PID, etc)
static void *pellet_func(void *arg)
{
	// Storing thread ID for logging
	int tid = pthread_self();

	// Randomly chooses column to drop pellet
	int col = rand() % RIVER_WIDTH;

	// Locks pellet threads
	sem_wait(&thread_sem);
	char *pellet = shm + col;
    // Outputs pellet
	*pellet = 'o';
    // Increments counter by 1
	thread_counter++;
    // Unlocks semaphore
	sem_post(&thread_sem);
    // Sleeps(Waits) for 1 second
	sleep(1);

	// Moves down the grid till the second to last row
	for(int i = 0; i < RIVER_HEIGHT - 2; i++)
    {
        // Locks pellet threads
		sem_wait(&thread_sem);
        // Outputs river
		*pellet = '~';
		pellet += RIVER_WIDTH;
        // Outputs pellet
		*pellet = 'o';
        // Increments counter by 1
		thread_counter++;
        // Unlocks semaphore
		sem_post(&thread_sem);
        // Sleeps(Waits) for 1 second
		sleep(1);
	}
    // Locks pellet threads
	sem_wait(&thread_sem);
    // Outputs river
	*pellet = '~';
	pellet += RIVER_WIDTH;

	// Checks to see if fish is below pellet, if so pellet is eaten
	if(*pellet == 'F')
    {
		// Pellet counter
		eaten_counter++;
		// Opens Pellet Output.txt file and appends it
		fp2 = fopen("Pellet Output.txt", "a");
        // Outputs message
		fprintf(fp2, "\tPellet eaten (column: %d, tid: %d)\n", col, tid);
        // Outputs message
		fprintf(fp2, "\t\tTotal pellets eaten: %d\n", eaten_counter);
        // Closes file
		fclose(fp2);
        // Outputs message
		printf("Pellet eaten (column: %d)\n", col);
        // Decrements thread total counter
		thread_total--;
        // Unlocks semaphore thread
		sem_post(&thread_sem);
        // Return nothing
		return NULL;
	}
    // Goes here if fish is NOT directly below pellet
	else
    {
		// Pellet was not eaten and missed
		*pellet = 'o';
	}
    // Increments thread counter
	thread_counter++;
    // Unlocks semaphore thread
	sem_post(&thread_sem);
    // Sleeps(Waits) for 1 second
	sleep(1);
	// Locks semmpahore thread
	sem_wait(&thread_sem);
    // Outputs river
	*pellet = '~';
    // Increments thread counter
	thread_counter++;
    // Unlocks seamphore thread
	sem_post(&thread_sem);
	// Opens Pellet Output.txt file and appends it
	fp2 = fopen("Pellet Output.txt", "a");
    // Outputs message
	fprintf(fp2, "\tPellet missed (column: %d, tid: %d)\n", col, tid);
    // Closes file
	fclose(fp2);
    // Outputs message
	printf("Pellet missed (column: %d)\n", col);
    // Decrements thread total count
	thread_total--;
    // Return nothing
	return NULL;
}
