/*
Now write a program that uses wait() to 
wait for the child process to finish in the parent. 
What does wait() return? 
What happens if you use wait() in the child?
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
        int rc_wait = wait(NULL);
        printf("child (pid:%d) (rc_wait:%d)\n", (int)getpid(), rc_wait);
    }
    else // parent goes down this path
    {
        int rc_wait = wait(NULL); // parent blocks until child finishes
        printf("parent of %d (rc_wait:%d) (pid:%d)\n",
               rc, rc_wait, (int)getpid());
    }
    return 0;
}

/*
- wait() returns the child process ID.
- wait() in child returns -1 (the child process has no children)
*/