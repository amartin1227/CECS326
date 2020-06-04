/*
Alejandro Martin
014593967
Lab 1 - Threads in C
*/

//libraries to include into program for functions, such as "printf".
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

/*
Q1:
Pthread in this lab is used to create a new thread in the calling process. By storing the ID of 
a new thread in buffer pointed to the thread, we are able to call pthread_create().
To terminate our child thread, we use the pthread_cancel().Pthread_cancel() is only called and used
once the user inputs the "enter" key. The child thread is continuous and will continue to run
until the enter key is inputted. 
Q2:
The sleep thread can continue printing until the keyboard input is pressed because 
sleep is a system call that causes the caller to block or be suspended until another
process wakes it up. In this case, the process that wakes it up is the user pressing
the enter key on keyboard. 
*/

//
static void *child(void *ignored){
	/*
	print statement to start program
	sleep(x) causes calling thread to sleep until the number of 
	real-time seconds specified in seconds have elapsed
	*/
	printf("\nWelcome all!\n"); 
	sleep(1);
	printf("Threading is now commencing in ...\n");
	sleep(1);
	printf("3\n");
	sleep(1);
	printf("2\n");
	sleep(1);
	printf("1\n");
	sleep(1);	
	//In this while loop, the thread keeps running.
	//It has a 3 second 'sleep' delay during the continuous while loop
	while(1){
		sleep(3);
		printf("Child is done sleeping 3 seconds. \n");
	}
	return NULL;
}


//Main function that does most of the work in the program
//It creates a new thread and has while loop for once the enter bar is pressed on keyboard
//Also shows termination of program
int main(){

	//creates new child thread of type thread
	pthread_t child_thread;
	
	// created to hold the user's keyboard input, in this case the enter
	char c;
	int code;

	//New thread runs and it will run and create child thread
	code = pthread_create(&child_thread, NULL, child, NULL);

	//prints error if something wrong occurs
	if(code){
		fprintf(stderr, "pthread_create failed with code %d\n", code);
	}
	
	/*
	  while loop will continue running until the  enter bar is pressed
	  once enter bar is pressed, then program enters the if loop
	  once in the if loop, program then outputs the cancellation of the program
	*/
	while(1){
		c = getchar();
		if(c == '\n'){
			//cancelling thread
			pthread_cancel(child_thread);
			printf("Child thread is now being cancelled...\n");
			break;			
		}
	}
	
	//Program then waits 5 seconds and displays the ending message of the program
	sleep(5);
	printf("Be careful... parent is awake!\n");
	printf("Congrats, you just terminated Lab 1.\n");
	printf("Please come again! :] \n\n");
}	
