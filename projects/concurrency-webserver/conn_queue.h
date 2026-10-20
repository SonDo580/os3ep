#ifndef __conn_queue_h__
#define __conn_queue_h__

#include "common_threads.h"

typedef struct __ConnQueue
{
    int *buf;
    int size;

    int fill_ptr; // where to put next item
    int use_ptr;  // where to get next item
    int count;    // number of filled items

    pthread_mutex_t mutex;
    pthread_cond_t empty;
    pthread_cond_t fill;
} ConnQueue;

void conn_queue_init(ConnQueue *q, int size);
void conn_queue_free(ConnQueue *q);
void conn_queue_produce(ConnQueue *q, int connfd);
int conn_queue_consume(ConnQueue *q);

#endif