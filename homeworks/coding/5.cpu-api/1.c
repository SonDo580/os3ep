/*
Write a program that calls fork().
Before calling fork(), have the main process access a variable (e.g., x)
and set its value to something (e.g., 100).
What value is the variable in the child process?
What happens to the variable when both the child and parent change the value of x?
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    int x;
    x = 100;
    printf("(before fork) x: %d\n", x);

    int rc = fork();
    if (rc < 0) // fork failed
    {
        fprintf(stderr, "fork failed\n");
        exit(EXIT_FAILURE);
    }
    else if (rc == 0) // child (new) process
    {
        printf("(child) x: %d\n", x);
        x += 1;
        printf("(child) updated x: %d\n", x);
    }
    else // parent goes down this path
    {
        // wait(NULL); // parent blocks until child complete
        x += 1;
        printf("(parent) updated x: %d\n", x);
    }

    return 0;
}

/*
Prediction:
- child got its own copy of the address space (its own private memory),
  so updating x in parent should not affect x in child,
  and vice versa.

Without 'wait' (either can run first after fork):
(before fork) x: 100
(parent) updated x: 101
(child) x: 100
(child) updated x: 101

With 'wait' (parents blocks until child completes):
(before fork) x: 100
(child) x: 100
(child) updated x: 101
(parent) updated x: 101
*/
