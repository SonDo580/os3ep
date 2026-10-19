#include "thread_pool.h"
#include "common.h"
#include "request.h"
#include "io_helper.h"

static void *worker(void *arg)
{
    ConnQueue *q = (ConnQueue *)arg;
    while (1)
    {
        int connfd = conn_queue_consume(q);
        request_handle(connfd);
        close_or_die(connfd);
    }
}

void thread_pool_init(ThreadPool *tp, int size, ConnQueue *q)
{
    assert(size > 0 && size <= MAX_THREADS);
    tp->size = size;
    tp->pids = Malloc(tp->size * sizeof(pthread_t));

    for (int i = 0; i < tp->size; i++)
        Pthread_create(&tp->pids[i], NULL, worker, (void *)q);
}