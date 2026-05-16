/*
A program that uses a certain amount of memory.
Should take 1 command-line argument: the number of megabytes of memory it will use.

When run, it should allocate an array, and constantly stream through the array,
touching each entry.

The program should do this indefinitely, or, perhaps,
for a certain amount of time also specified at thecommand line.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: memory-user <MiB>.\n");
        exit(EXIT_FAILURE);
    }

    int MiB = atoi(argv[1]); // can overflow and become negative
    assert(MiB > 0);

    int nbytes = MiB * (1 << 20);
    char *array = (char *)malloc(nbytes);
    assert(array != NULL);

    printf("PID: %d\n", (int)getpid());

    // infinite loop to "touch" entries
    for (int i = 0;; i++)
    {
        if (i == nbytes)
            i = 0; // wrap around
        array[i] = 'a';
    }
}