# CECS326

# Lab1
- Read the documentation for pthread_cancel by typing ’man pthread_cancel’
(without the quotes) on the Linux command line. Using this information and the model provided below,
write a program where the initial (main) thread creates a second thread. The main thread should read input
from the keyboard, waiting until the user presses the Enter key. At that point, it should kill off the second
thread and print out a message reporting that it has done so. Meanwhile, the second thread should be in an
infinite loop, each time around sleeping 3 seconds and then printing out a message. Once the enter key has
been pressed, the main thread should wait for 5 seconds to demonstrate that the second thread has been
successfully killed and then exit.
This program should be coded in C, using the gcc C compiler on POSIX (i.e. Linux or Mac OS) systems.
Answer the following questions in a long-form comment at the top of your code:
1. Explain what pthread is doing in this program. Be specific.
2. Explain why the sleeping thread can print its periodic messages while the main thread is waiting for
keyboard input.

# Lab2
- You will write a program that uses multiple processes to simulate a swim mill to show the behavior of a
fish swimming upstream. The swim mill is represented as a one-dimensional array such that each element
indicates the distance from the shore. The fish will occupy one and only one of these elements. The presence
of a fish is indicated by changing the integer in the specified element. For example, we can represent the fish
by the character F. By default, we will have the fish swimming right in the middle of the stream.
Somewhere upstream, a pellet is dropped into the stream. Our fish sees this pellet traveling towards it
and moves sideways to enable its capture. If the pellet and the fish are in the same position at any point,
the fish is said to have eaten the pellet. Of course, it is possible for our fish to miss the pellet if the fish
cannot move close to it in time.
Different types of processes needed for this project are as follows:
• A set of pellet processes: Each pellet process drops a pellet at a random distance from the fish. We
will call this process pellet. The pellet will start moving towards the fish with the flow of the river.
For simplicity, we will assume that the pellet moves in a straight line downstream.
• A process called fish: the fish sees the pellet as it is dropped and moves one unit left or right to start
aligning itself with the pellet. As the pellet moves towards the fish and the fish is at the same distance
from the shore as the pellet, it stops changing its sideways position and devours the pellet when they are
coincident. After eating the pellet, or if the pellet is missed, the fish returns to swimming midstream.
• A coordinator process named swim_mill: It is responsible for creating the fish process, the pellet
processes, and coordinating the termination of pellets. We could have more than one pellet at any
time. Note that the position of pellet and fish are maintained by their respective processes.
The swim_mill process also sets a timer at the start of computation to 30 seconds. If computation has not
finished by this time, swim_mill kills all the children and grandchildren, and then exits. Make sure that
you print appropriate message(s).
In addition, swim_mill should print a message when an interrupt signal (^C) is received. Make sure
that all the children/grandchildren are killed by swim_mill when this happens, and all the shared memory is
deallocated. The grandchildren kill themselves upon receiving interrupt signal but print a message on stderr
to indicate that they are dying because of an interrupt, along with the identification information. Make sure
that the processes handle multiple interrupts correctly. As a precaution, add this feature only after your
program is well debugged.
Implementation. The code for pellet, fish, and swim_mill processes should be compiled separately and
the executables be named pellet, fish, and swim_mill, respectively. The program should be executed by
calling: swim_mill.
Each pellet process prints its process id, its position and whether it was eaten or missed, before exiting.
Each time the coordinator gets a result from a pellet, it prints the pid of the pellet.
swim_mill will set up the two-dimensional array in shared memory, and write the result into a file after
each child is finished. The fish will be restricted to move in the last row of the 2D array. The swim_mill
process will create a pellet at random intervals, with the pellet being dropped into the swim mill at a random distance from the fish. The process pellet will increment its position and will terminate after the pellet has
reached the last row, whether it is eaten or not.
The pellet can be represented by 0x80 and can be moved to location (x, y) by the statement L[x][y] |=
0x80. This will allow the pellet and the fish to be able to coincide. The process of eating is performed by
getting rid of the most significant bit to return the fish to original state.
The process fish will scan the entire array and will focus on a pellet if it detects one, to arrange itself in
its line of drift. Meanwhile, if another pellet is found that happens to be closer, and eatable, the fish will go
for it.
Other points to remember: You are required to use fork, exec (or one of its variants), wait, and exit to
manage multiple processes. Use shmctl suite of calls for shared memory allocation. Also make sure that you
do not have more than twenty processes in the system at any time. You can do this by keeping a counter in
swim_mill that gets incremented by fork and decremented by wait.
I’m also requiring the use of a makefile for your compilation. It will save you a lot of time compiling and
re-compiling all three source code files and is very common for source code downloaded from FOSS projects
and larger-scale, complex coding. A makefile is a simple script that contains all of the compilation executions
and the options for each file’s compilation. For more information on makefiles, read this short tutorial here:
http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor.

# Lab3
- Your assignment
is to modify swim_mill which you created with Assignment #2 to include a semaphore to control access
to critical resources.
Make sure that you have signal handing to terminate all processes, if needed. In case of abnormal
termination, make sure to remove shared memory and semaphores as well. Use semget(2), semctl(2), and
semop(2) to implement the semaphore.
Your semaphore should receive the names of functions passed as parameters. Since different functions
are required to access the critical resource, passing the functions as parameters will prove a clean option.
