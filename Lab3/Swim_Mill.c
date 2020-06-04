/*
    Alejandro Martin
    CECS326
    Lab 3
*/

// Libraries to include in program
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

// Constant is equal to 11 x 11 = 121
static const int RIVER_SIZE = 121;

// Semaphore declaration of swim_mill
sem_t *mill_sem;
// Semaphore declaration of fish
sem_t *fish_sem;
// Semaphore declaration of pellet
sem_t *pellet_sem;

// Method instances
void printMem(char *shm);
void interruptAndExit();
void killAndExit();

// Sets up and identifies shared memory.
int shmid;
key_t shm_key = 1234;

// Points to start of shared memory
char *shm, *river;

// Store process IDs for the three processes
pid_t fish;
pid_t pellet;
pid_t pid;

// File to write log process information to
FILE *fp;

// Used when deallocating shared memory
struct shmid_ds buf;

// Main Function
int main()
{
    // Gets process' process ID
	pid = getpid();

	// Opens process output and writes initial information
	fp = fopen("Process Output.txt", "w");
    // Outputs message
	fprintf(fp, "Creating the processes:\n");
    // Outputs message with process' process ID
	fprintf(fp, "\tSwim mill process has started (pid: %d)\n", pid);

	// Interrupt signal
    // SIGINT - program is interrupted by pressing CTR + C
    // Goes to interruptAndExit function
	signal(SIGINT, interruptAndExit);

	// Set flag in shmget to perform usual functions, '0666' sets
    // access permissions of the memory segment, and 'IPC_CREAT'
    // tells system to create new memory segment for the shared memory
	if((shmid = shmget(shm_key, RIVER_SIZE, IPC_CREAT | 0666)) < 0)
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
    
    // Outputs message to Process Output.txt file stating shared memory being created
	fprintf(fp, "\tShared memory was created (shmid: %d)\n", shmid);

	// Named sempahores for all swim_mill, fish, and pellet are created to
    // control all processes. Multiple semaphores are used since each one
    // controls a single process. Once a process is finished, the next
    // semaphore of the next process is raised so that process can run.
	mill_sem = sem_open("/mill_sem", O_CREAT, S_IRWXU | S_IRWXO, 1);
	fish_sem = sem_open("/fish_sem", O_CREAT, S_IRWXU | S_IRWXO, 0);
	pellet_sem = sem_open("/pellet_sem", O_CREAT, S_IRWXU | S_IRWXO, 0);

	// Fills up river shared memory with '~'
	river = shm;
    // For as long as it is less than the height of the river(11)
    // and shorter than width of river width(11)
    // a '~' will be printed onto terminal
	for(int i = 0; i < RIVER_HEIGHT; i++)
    {
		for(int j = 0; j < RIVER_WIDTH; j++)
        {
			*river = '~';
			river++;
		}
	}
	river = shm;

	// Fish process is created
	fish = fork();
	if(fish == 0)
    {
		const char *arg;
        // Overlays process image that has been created by fork() function with
        // filename of Fish file that has executable image of the new process
        // and is a character length list of arguments that must end with NULL pointer
        execl("./FISH", arg, (char *) NULL);
	}
    // Outputs message to Process Output.txt file stating fish process is starting
    // with its process' process ID
	fprintf(fp, "\tFish process has started (pid: %d)\n", fish);

	// Pellet process is created
	pellet = fork();
	if(pellet == 0)
    {
		const char *arg;
        // Overlays process image that has been created by fork() function with
        // filename of Fish file that has executable image of the new process
        // and is a character length list of arguments that must end with NULL pointer
		execl("./PELLET", arg, (char *) NULL);
	}
    // Outputs message to Process Output.txt file stating pellet process is starting
    // with its process' process ID
	fprintf(fp, "\tPellet process has started (pid: %d)\n", pellet);

	// As long as there is time left, river bank is printed
	for(int t = TIME; t > 0; t--)
    {
		// Locks pellet threads
		sem_wait(mill_sem);
        // Outputs time remaining above river bank
		printf("\nTime remaining: %d\n", t);
        // prints shared memory of river bank
		printMem(shm);
        // Unlocks fish_sem so it can run
		sem_post(fish_sem);
        // Sleeps(Waits) for 1 second
		sleep(1);
	}
    
    // Outputs that simulation is done
    printf("Time has now ended.\n");
    // Outputs that simulation is done
    printf("Program is now complete.\n");
    // Goes to killAndExit function
    killAndExit(0);
    // Exits main function
    return(0);
}

// Loops through and prints the contents of the shared memory
void printMem(char *shm)
{
    // Loops through river bank
	for(int i = 0; i < RIVER_HEIGHT; i++)
    {
		for(int j = 0; j < RIVER_WIDTH; j++)
        {
			char c = *shm;
			shm++;
			putchar(c);
            // Puts space character
			putchar(' ');
		}
        // Puts newline character
		putchar('\n');
	}
    // Puts newline character
	putchar('\n');
}

// This function is executed when interrupt signal is pressed
void interruptAndExit()
{
    // Outputs message that interrupt button was pushed and was received
	fprintf(stderr, "\nInterrupt was pushed and received.\n");
    // Runs to killAndExit() function
	killAndExit();
}

// Closes(kills) all processes to end the program when 30 seconds are
// over with, or when the interrupt signal is pressed. All outputs below
// are written to Process Output.txt file
void killAndExit()
{
    // Outputs message
	fprintf(fp, "\nKilling the children processes and exiting swim mill:\n");
	// Kills fish child process
	kill(fish, SIGTERM);
    // Outputs message
	fprintf(fp, "\tFish process has been killed (pid: %d)\n", fish);
    // Kills pellet child process
	kill(pellet, SIGTERM);
    // Outputs message
	fprintf(fp, "\tPellet process has been killed (pid: %d)\n", pellet);
	// Outputs message
	fprintf(fp, "\tDeallocating the shared memory (shmid: %d)\n", shmid);
	// Detaches shared memory from program
    shmdt(shm);
    // Removes shared memory segment
    shmctl(shmid, IPC_RMID, &buf);
    // Outputs message
	fprintf(fp, "\tRemoving the semaphore.\n");
    // Closes swim_mill semaphore
	sem_close(mill_sem);
    // Removes swim_mill semaphore
	sem_unlink("/mill_sem");
    // Closes fush semaphore
	sem_close(fish_sem);
    // Removes fish semaphore
	sem_unlink("/fish_sem");
    // Closes pellet semaphore
	sem_close(pellet_sem);
    // Removes pellet semaphore
	sem_unlink("/pellet_sem");
    // Outputs message for exiting
	fprintf(fp, "\tExiting the swim mill (pid: %d)\n", pid);
    // Closes file Process Output.txt
	fclose(fp);
    // Exits function
	exit(0);
}
