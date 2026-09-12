/* simple race condition: access shared variable without locking. */

#include <stdio.h>

#include "common_threads.h"

int balance = 0;

void *worker(void *arg)
{
    balance++; // unprotected access
    return NULL;
}

int main()
{
    pthread_t p;
    Pthread_create(&p, NULL, worker, NULL);
    balance++; // unprotected access
    Pthread_join(p, NULL);
    return 0;
}