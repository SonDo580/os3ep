#include "thread_pool.h"
#include "common.h"
#include "request.h"
#include "io_helper.h"

static void *worker(void *arg)
{
    ThreadPool *tp = (ThreadPool *)arg;
    while (1)
    {
        if (strcmp(tp->sched_algo, SchedAlgoFIFO) == 0)
        {
            int connfd = conn_queue_consume(tp->q);
            RequestInfo req;
            request_handle(connfd, &req);
            close_or_die(connfd);
        }
        else if (strcmp(tp->sched_algo, SchedAlgoSFF) == 0)
        {
            ConnHeapItem item = conn_heap_consume(tp->h);
            request_handle_after_http_line(item.connfd, item.req);
            close_or_die(item.connfd);
        }
    }
}

void thread_pool_init(ThreadPool *tp, int size, char *sched_algo, ConnQueue *q, ConnHeap *h)
{
    assert(size > 0 && size <= MAX_THREADS);
    tp->size = size;
    tp->pids = Malloc(tp->size * sizeof(pthread_t));

    tp->sched_algo = sched_algo;
    if (strcmp(sched_algo, SchedAlgoFIFO) == 0)
    {
        assert(q != NULL);
        tp->q = q;
    }
    else if (strcmp(sched_algo, SchedAlgoSFF) == 0)
    {
        assert(h != NULL);
        tp->h = h;
    }

    for (int i = 0; i < tp->size; i++)
        Pthread_create(&tp->pids[i], NULL, worker, (void *)tp);
}
