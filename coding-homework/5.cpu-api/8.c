/*
Write a program that creates two children,
and connects the standard output of one to the standard input of the other,
using the pipe() system call.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    int pipedes[2];
    if (pipe(pipedes) == -1)
    {
        fprintf(stderr, "pipe() failed\n");
        exit(EXIT_FAILURE);
    }
    // bytes written on pipedes[1] can be read from pipedes[0]

    int rc1 = fork();
    if (rc1 < 0) // fork failed
    {
        fprintf(stderr, "fork 1 failed\n");
        exit(EXIT_FAILURE);
    }
    else if (rc1 == 0) // child 1
    {
        // make stdout of child 1 write to the pipe
        dup2(pipedes[1], STDOUT_FILENO);
        // int dup2(int __fd, int __fd2)
        // . close fd2, then make fd2 open on the the same file as fd

        // close unused FDs
        close(pipedes[0]); // read end
        close(pipedes[1]); // write end, already handled by STDOUT_FILENO

        printf("(child 1) sent: Hello\n");
        fflush(stdout);       // flush data into the pipe
        close(STDOUT_FILENO); // read end will receive EOF 
    }
    else // parent goes down this path
    {
        int rc2 = fork();
        if (rc2 < 0) // fork failed
        {
            fprintf(stderr, "fork 2 failed\n");
            exit(EXIT_FAILURE);
        }
        else if (rc2 == 0) // child 2
        {
            // make stdin of child 2 read from the pipe
            dup2(pipedes[0], STDIN_FILENO);

            // close unused FDs
            close(pipedes[1]); // write end
            close(pipedes[0]); // read end, already handled by STDIN_FILENO

            printf("(child 2) received: ");
            int c;
            while ((c = getchar()) != EOF)
                putchar(c);
        }
        else // parent goes down this path
        {
            // parent don't use the pipe -> close them
            // (otherwise the pipe's write-end reference count will not become 0,
            //  and child 2 will never receive EOF)
            close(pipedes[0]);
            close(pipedes[1]);

            // wait for both children
            int status1;
            int status2;
            waitpid(rc1, &status1, 0);
            waitpid(rc2, &status2, 0);
            printf("(parent) both children finished\n");
        }
    }

    return 0;
}