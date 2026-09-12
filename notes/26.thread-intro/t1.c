#include <stdio.h>
#include <pthread.h>

#include "common.h"
#include "common_threads.h"

// volatile forces reads/writes directly from/to RAM, 
// preventing compiler optimization (cache variable in register).
// -> operation is not atomic (not thread-safe).
static volatile int counter = 0;

// add 1 to counter repeatedly in a loop
void *mythread(void *arg)
{
    printf("%s: begin\n", (char *)arg);
    for (int i = 0; i < 1e7; i++)
        counter = counter + 1; // TODO: can we use counter++
    printf("%s: done\n", (char *)arg);
    return NULL;
}

int main()
{
    pthread_t p1, p2;
    printf("main: begin (counter = %d)\n", counter);
    Pthread_create(&p1, NULL, mythread, "A");
    Pthread_create(&p2, NULL, mythread, "B");

    Pthread_join(p1, NULL);
    Pthread_join(p2, NULL);
    printf("main: done with both (counter = %d)\n", counter);
    return 0;
}