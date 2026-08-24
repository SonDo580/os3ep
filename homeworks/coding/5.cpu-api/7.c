/*
Write a program that creates a child process, 
and then in the child closes standard output (STDOUT_FILENO). 
What happens if the child calls printf() 
to print some output after closing the descriptor?
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    int rc = fork();
    if (rc < 0) // fork failed
    {
        fprintf(stderr, "fork failed\n");
        exit(EXIT_FAILURE);
    }
    else if (rc == 0) // child (new process)
    {
        close(STDOUT_FILENO); // close standard output
        printf("(child) after stdout closed\n");
    }

    return 0;
}

/*
printf() doesn't print anything to the screen after stdout is closed
*/