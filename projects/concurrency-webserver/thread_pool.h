#ifndef __thread_pool_h_
#define __thread_pool_h_

#include "common_threads.h"
#include "conn_queue.h"
#include "conn_heap.h"

#define MAX_THREADS 100

typedef struct __ThreadPool
{
    pthread_t *pids;
    int size;

    char *sched_algo;
    ConnQueue *q;
    ConnHeap *h;
} ThreadPool;

void thread_pool_init(ThreadPool *tp, int size, char *sched_algo, ConnQueue *q, ConnHeap *h);

#endif
