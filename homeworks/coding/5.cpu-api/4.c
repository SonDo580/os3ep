/*
Write a program that calls fork() and then calls some form of
exec() to run the program /bin/ls. 
See if you can try all of the variants of exec(), including (on Linux) 
execl(), execle(), execlp(), execv(), execvp(), and execvpe(). 
Why do you think there are so many variants of the same basic call?
*/

#define _GNU_SOURCE // needed to use execvpe
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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
        // char *args[] = {"/bin/ls", "-la", ".", NULL};
        // execv(args[0], args);

        // char *args[] = {"ls", "-la", ".", NULL};
        // execvp(args[0], args);
        
        // char *args[] = {"ls", "-la", ".", NULL};
        // char *env[] = {NULL};
        // execvpe(args[0], args, env);

        // execl("/bin/ls", "ls", "-la", ".", NULL);
        
        // execlp("ls", "ls", "-la", ".", NULL);
        
        char *env[] = {NULL};
        execle("/bin/ls", "ls", "-la", ".", NULL, env);

        printf("(child): should not reached here\n");
    }

    return 0;
}

/* exec()
Core task: replace current process with a new process

Variants:
- l (list): arguments are passed as a comma-separated list.
  . useful if arguments are fixed.
- v (vector): arguments are passed as an array of strings.
  . needed if we don't know arguments in advance
  . example: user types a command into a shell -> parse to array of args
- p (path): OS will search the PATH environment variable to find the executable.
  . use: convenient, but searching takes time
  . omit: faster, more secure
- e (environment): provide a custom array of environment variables.
  . provide restricted environment so new process can't access sensitive system variables. 
*/