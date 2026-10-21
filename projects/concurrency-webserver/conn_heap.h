#ifndef __conn_heap__
#define __conn_heap__

#include <sys/stat.h>

#include "common_threads.h"
#include "request.h"

typedef struct __ConnHeapItem
{
    off_t filesize; // (comparison key)
    int connfd;
    RequestInfo *req;
} ConnHeapItem;

typedef struct __ConnHeap
{
    ConnHeapItem *items;
    int size;
    int count;

    pthread_mutex_t mutex;
    pthread_cond_t empty;
    pthread_cond_t fill;
} ConnHeap;

void conn_heap_init(ConnHeap *h, int size);
void conn_heap_produce(ConnHeap *h, ConnHeapItem item);
ConnHeapItem conn_heap_consume(ConnHeap *h);

#endif