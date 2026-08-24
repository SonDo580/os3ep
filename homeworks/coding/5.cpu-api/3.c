/*
Write another program using fork().
The child process should print “hello”;
the parent process should print “goodbye”.
You should try to ensure that the child process always prints first;
can you do this without calling wait() in the parent?
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main()
{

    int rc = fork();
    if (rc < 0) // fork failed
    {
        fprintf(stderr, "fork failed\n");
        exit(EXIT_FAILURE);
    }
    else if (rc == 0) // child (new) process
    {
        printf("(child) hello\n");
    }
    else // parent goes down this path
    {
        wait(NULL); // blocks until child completes
        printf("(parent) goodbye\n");
    }

    return 0;
}

/*
- we cannot ensure that parent always prints later without wait()
*/