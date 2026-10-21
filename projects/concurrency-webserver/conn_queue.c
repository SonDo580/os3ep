#include <assert.h>

#include "conn_queue.h"
#include "common.h"

void conn_queue_init(ConnQueue *q, int size)
{
    assert(size > 0 && size <= MAX_QUEUE_SIZE);
    q->size = size;
    q->buf = Malloc(q->size * sizeof(int));

    q->fill_ptr = 0;
    q->use_ptr = 0;
    q->count = 0;

    Pthread_mutex_init(&q->mutex, NULL);
    Pthread_cond_init(&q->fill, NULL);
    Pthread_cond_init(&q->empty, NULL);
}

static void conn_queue_put(ConnQueue *q, int connfd)
{
    q->buf[q->fill_ptr] = connfd;
    q->fill_ptr = (q->fill_ptr + 1) % q->size;
    q->count++;
}

static int conn_queue_get(ConnQueue *q)
{
    int connfd = q->buf[q->use_ptr];
    q->use_ptr = (q->use_ptr + 1) % q->size;
    q->count--;
    return connfd;
}

void conn_queue_produce(ConnQueue *q, int connfd)
{
    Pthread_mutex_lock(&q->mutex);
    while (q->count == q->size)
        Pthread_cond_wait(&q->empty, &q->mutex);

    conn_queue_put(q, connfd);

    Pthread_cond_signal(&q->fill);
    Pthread_mutex_unlock(&q->mutex);
}

int conn_queue_consume(ConnQueue *q)
{
    Pthread_mutex_lock(&q->mutex);
    while (q->count == 0)
        Pthread_cond_wait(&q->fill, &q->mutex);

    int connfd = conn_queue_get(q);

    Pthread_cond_signal(&q->empty);
    Pthread_mutex_unlock(&q->mutex);

    return connfd;
}