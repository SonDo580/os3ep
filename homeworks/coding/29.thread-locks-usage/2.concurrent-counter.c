#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/time.h>

#include "common_threads.h"

// ===== Counter =====

typedef struct __counter_t
{
    int value;
    pthread_mutex_t lock;
} counter_t;

void init(counter_t *c)
{
    c->value = 0;
    Pthread_mutex_init(&c->lock, NULL);
}

void increment(counter_t *c)
{
    Pthread_mutex_lock(&c->lock);
    c->value++;
    Pthread_mutex_unlock(&c->lock);
}

void decrement(counter_t *c)
{
    Pthread_mutex_lock(&c->lock);
    c->value--;
    Pthread_mutex_unlock(&c->lock);
}

int get(counter_t *c)
{
    Pthread_mutex_lock(&c->lock);
    int rc = c->value;
    Pthread_mutex_unlock(&c->lock);
    return rc;
}

// ===== Measurement =====

#define N 1000000

typedef struct __WorkerArgs
{
    counter_t *c;
    int thread_num;
} WorkerArgs;

void *worker(void *arg)
{
    WorkerArgs *args = (WorkerArgs *)arg;
    counter_t *c = args->c;
    struct timeval start, end;

    gettimeofday(&start, NULL);

    for (int i = 0; i < N; i++)
        increment(c);

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
    int num_cpus = (int)sysconf(_SC_NPROCESSORS_ONLN);
    assert(num_cpus > 0);
    printf("Number of online CPU: %d\n", num_cpus);

    counter_t *c = malloc(sizeof(counter_t));
    assert(c != NULL);
    init(c);

    printf("Each thread will increment counter %d times\n\n", N);
    for (int nthreads = 1; nthreads <= 2 * num_cpus; nthreads++)
        test(c, nthreads);

    return 0;
}

/*
Number of online CPU: 4
Each thread will increment counter 1000000 times

=== Concurrent threads: 1 ===
Thread 0: 174927 microseconds
Average: 174927 microseconds

=== Concurrent threads: 2 ===
Thread 1: 289013 microseconds
Thread 0: 289222 microseconds
Average: 289117 microseconds

=== Concurrent threads: 3 ===
Thread 2: 179054 microseconds
Thread 0: 371190 microseconds
Thread 1: 374686 microseconds
Average: 308310 microseconds

=== Concurrent threads: 4 ===
Thread 3: 550559 microseconds
Thread 1: 595113 microseconds
Thread 0: 654612 microseconds
Thread 2: 657387 microseconds
Average: 614417 microseconds

=== Concurrent threads: 5 ===
Thread 2: 549033 microseconds
Thread 3: 626751 microseconds
Thread 4: 737551 microseconds
Thread 0: 812953 microseconds
Thread 1: 813478 microseconds
Average: 707953 microseconds

=== Concurrent threads: 6 ===
Thread 3: 797495 microseconds
Thread 4: 969768 microseconds
Thread 5: 987520 microseconds
Thread 2: 994955 microseconds
Thread 1: 1014151 microseconds
Thread 0: 1018953 microseconds
Average: 963807 microseconds

=== Concurrent threads: 7 ===
Thread 3: 979544 microseconds
Thread 0: 1014615 microseconds
Thread 5: 1117310 microseconds
Thread 6: 1125199 microseconds
Thread 2: 1135412 microseconds
Thread 4: 1146743 microseconds
Thread 1: 1151007 microseconds
Average: 1095690 microseconds

=== Concurrent threads: 8 ===
Thread 6: 1067962 microseconds
Thread 2: 1139783 microseconds
Thread 3: 1272841 microseconds
Thread 5: 1285813 microseconds
Thread 7: 1302366 microseconds
Thread 4: 1305018 microseconds
Thread 0: 1318651 microseconds
Thread 1: 1330824 microseconds
Average: 1252907 microseconds
*/

/* 
Observations:
- As the number of threads increases, each thread takes longer (on average) 
  to increase the counter N times.

Reasons:
- If the number of threads is greater than the number of CPUs,
  we have context switching overhead (even if threads yield the CPU right away).
- ...
*/