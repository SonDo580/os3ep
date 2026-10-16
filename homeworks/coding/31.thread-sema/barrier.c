/*
Barrier synchronization:
- Assume there are 2 points between in a piece of code - P1 and P2.
- Putting a barrier between P1 and P2 guarantees that all threads
  execute P1 before any thread executes P2.

Expected result:
- Each child should print "before" before either prints "after".
*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "common_threads.h"

typedef struct __barrier_t
{
    int total_threads;   // total number of threads
    int entered_threads; // number of threads that have go past P1
    sem_t mutex;         // binary semaphore
    sem_t sem;
} barrier_t;

/*
Idea:
- Init sem's value to 0.
- After go past P1, each thread acquires the lock and increments entered_threads,
  then releases the lock and calls sem_wait(&sem).
- The last thread going past P1 should call sem_post(&sem) multiple times
  to unblock other threads.
*/

barrier_t b; // the single barrier used in this program

void barrier_init(barrier_t *b, int num_threads)
{
    b->total_threads = num_threads;
    b->entered_threads = 0;
    Sem_init(&b->mutex, 0, 1);
    Sem_init(&b->sem, 0, 0);
}

void barrier(barrier_t *b)
{
    Sem_wait(&b->mutex);
    b->entered_threads++;
    Sem_post(&b->mutex);
    int is_last_thread = b->entered_threads == b->total_threads;

    if (is_last_thread)
    {
        for (int i = 0; i < b->total_threads; i++)
        {
            Sem_post(&b->sem);
        }
    }

    if (!is_last_thread)
    {
        Sem_wait(&b->sem);
    }
}

typedef struct __tinfo_t
{
    int thread_id;
} tinfo_t;

void *child(void *arg)
{
    tinfo_t *t = (tinfo_t *)arg;
    printf("child %d: before\n", t->thread_id);
    barrier(&b);
    printf("child %d: after\n", t->thread_id);
    return NULL;
}

int main(int argc, char *argv[])
{
    assert(argc == 2);
    int num_threads = atoi(argv[1]);
    assert(num_threads > 0);

    pthread_t p[num_threads];
    tinfo_t t[num_threads];

    printf("parent: begin\n");
    barrier_init(&b, num_threads);

    int i;
    for (i = 0; i < num_threads; i++)
    {
        t[i].thread_id = i;
        Pthread_create(&p[i], NULL, child, &t[i]);
    }

    for (i = 0; i < num_threads; i++)
        Pthread_join(p[i], NULL);

    printf("parent: end\n");
    return 0;
}