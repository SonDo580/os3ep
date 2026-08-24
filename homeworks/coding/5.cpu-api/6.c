/*
Write a slight modification of the previous program,
this time using waitpid() instead of wait().
When would waitpid() be useful?
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
        printf("child (pid:%d)\n", (int)getpid());
    }
    else // parent goes down this path
    {
        // wait for specific child with PID=rc
        int status;
        int option = 0;
        // int option = WNOHANG;
        int rc_wait = waitpid(rc, &status, option);

        printf("parent of %d (rc_wait:%d) (pid:%d)\n",
               rc, rc_wait, (int)getpid());
    }
    return 0;
}

/* waitpid()

Useful for:
- waiting for specific child
- polling (with option WNOHANG)
*/