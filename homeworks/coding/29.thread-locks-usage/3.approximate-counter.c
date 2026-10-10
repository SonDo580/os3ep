#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/time.h>

#include "common_threads.h"

// === Counter ===

#define NUMCPUS 4 // This is machine-specific
#define THRESHOLD 1024

typedef struct __counter_t
{
    int global;                     // global count
    pthread_mutex_t glock;          // global lock
    int local[NUMCPUS];             // per-CPU count
    pthread_mutex_t llock[NUMCPUS]; // ... and locks
    int threshold;                  // update frequency
} counter_t;

void init(counter_t *c, int threshold)
{
    c->threshold = threshold;
    c->global = 0;
    Pthread_mutex_init(&c->glock, NULL);
    for (int i = 0; i < NUMCPUS; i++)
    {
        c->local[i] = 0;
        Pthread_mutex_init(&c->llock[i], NULL);
    }
}

void update(counter_t *c, int threadID, int amount)
{
    int cpu = threadID % NUMCPUS;
    Pthread_mutex_lock(&c->llock[cpu]);

    // Update local counter
    c->local[cpu] += amount;

    if (c->local[cpu] >= c->threshold)
    {
        // Transfer to global counter if reached threshold
        Pthread_mutex_lock(&c->glock);
        c->global += c->local[cpu];
        Pthread_mutex_unlock(&c->glock);
        c->local[cpu] = 0; // reset
    }

    Pthread_mutex_unlock(&c->llock[cpu]);
}

int get(counter_t *c)
{
    // Return global counter
    Pthread_mutex_lock(&c->glock);
    int val = c->global;
    Pthread_mutex_unlock(&c->glock);
    return val; // only approximate
}

// === Measurement ===

#define N 1000000

typedef struct __WorkerArgs
{
    counter_t *c;
    int thread_num;
} WorkerArgs;

void *worker(void *arg)
{
    int tid = pthread_self();

    WorkerArgs *args = (WorkerArgs *)arg;
    counter_t *c = args->c;
    struct timeval start, end;

    gettimeofday(&start, NULL);

    for (int i = 0; i < N; i++)
        update(c, tid, 1);

    gettimeofday(&end, NULL);

    long long elapsed_usec = ((long long)end.tv_sec - start.tv_sec) * 1000000LL +
                             (end.tv_usec - start.tv_usec);

    printf("Thread %d: %lld microseconds\n", args->thread_num, elapsed_usec);

    return (void *)elapsed_usec;
}

void test(counter_t *c, int nthreads)
{
    printf("=== Concurrent threads: %d ===\n", nthreads);

    pthread_t threads[nthreads];
    void *usec[nthreads];

    for (int i = 0; i < nthreads; i++)
    {
        WorkerArgs *args = malloc(sizeof(WorkerArgs));
        assert(args != NULL);
        args->c = c;
        args->thread_num = i;
        Pthread_create(&threads[i], NULL, worker, (void *)args);
    }

    for (int i = 0; i < nthreads; i++)
        Pthread_join(threads[i], &usec[i]);

    long long average_usec = 0;
    for (int i = 0; i < nthreads; i++)
        average_usec += (long long)usec[i];
    average_usec /= nthreads;

    printf("Average: %lld microseconds\n", average_usec);
    printf("\n");
}

int main()
{

    counter_t *c = malloc(sizeof(counter_t));
    assert(c != NULL);
    init(c, THRESHOLD);

    printf("Each thread will increment counter %d times\n\n", N);
    for (int nthreads = 1; nthreads <= 2 * NUMCPUS; nthreads++)
        test(c, nthreads);

    return 0;
}

/*
Each thread will increment counter 1000000 times

=== Concurrent threads: 1 ===
Thread 0: 261200 microseconds
Average: 261200 microseconds

=== Concurrent threads: 2 ===
Thread 0: 218491 microseconds
Thread 1: 222615 microseconds
Average: 220553 microseconds

=== Concurrent threads: 3 ===
Thread 2: 322791 microseconds
Thread 1: 414531 microseconds
Thread 0: 429920 microseconds
Average: 389080 microseconds

=== Concurrent threads: 4 ===
Thread 1: 621545 microseconds
Thread 0: 708614 microseconds
Thread 3: 773421 microseconds
Thread 2: 776584 microseconds
Average: 720041 microseconds

=== Concurrent threads: 5 ===
Thread 0: 590951 microseconds
Thread 3: 646548 microseconds
Thread 2: 769403 microseconds
Thread 4: 877212 microseconds
Thread 1: 886951 microseconds
Average: 754213 microseconds

=== Concurrent threads: 6 ===
Thread 0: 739270 microseconds
Thread 2: 811752 microseconds
Thread 1: 1108834 microseconds
Thread 3: 1114985 microseconds
Thread 4: 1145248 microseconds
Thread 5: 1157327 microseconds
Average: 1012902 microseconds

=== Concurrent threads: 7 ===
Thread 1: 1155898 microseconds
Thread 2: 1271926 microseconds
Thread 4: 1281726 microseconds
Thread 6: 1330519 microseconds
Thread 0: 1332805 microseconds
Thread 5: 1353108 microseconds
Thread 3: 1369374 microseconds
Average: 1299336 microseconds

=== Concurrent threads: 8 ===
Thread 5: 1242273 microseconds
Thread 3: 1256766 microseconds
Thread 2: 1373093 microseconds
Thread 7: 1487694 microseconds
Thread 6: 1504354 microseconds
Thread 1: 1529490 microseconds
Thread 0: 1557433 microseconds
Thread 4: 1560326 microseconds
Average: 1438928 microseconds
*/

/*
Expected:
- If number of threads is roughly the same as the number of CPUs,
  performance should generally be the same as updating the counter with 1 thread,
  since threads across CPUs can update the local counter without contention.

Observations:
- ...

Reasons:
- ...
*/