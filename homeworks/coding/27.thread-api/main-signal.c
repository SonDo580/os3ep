/* simple child/parent signaling: without condition variable */

#include <stdio.h>

#include "common_threads.h"

int done = 0;

void *worker(void *arg)
{
    printf("this should print first\n");
    done = 1;
    return NULL;
}

int main()
{
    pthread_t p;
    Pthread_create(&p, NULL, worker, NULL);
    while (done == 0)
        ;
    printf("this should print last\n");
    return 0;
}