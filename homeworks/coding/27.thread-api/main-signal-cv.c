/* a more efficient signaling:ty via condition variable */

#include <stdio.h>

#include "common_threads.h"

typedef struct __synchronizer_t
{
    pthread_mutex_t lock;
    pthread_cond_t cond;
    int done;
} synchronizer_t;

synchronizer_t s;

void signal_init(synchronizer_t *s)
{
    Pthread_mutex_init(&s->lock, NULL);
    Pthread_cond_init(&s->cond, NULL);
    s->done = 0;
}

void signal_done(synchronizer_t *s)
{
    Pthread_mutex_lock(&s->lock);
    s->done = 1;
    Pthread_cond_signal(&s->cond);
    Pthread_mutex_unlock(&s->lock);
}

void signal_wait(synchronizer_t *s)
{
    Pthread_mutex_lock(&s->lock);
    while (s->done == 0)
        Pthread_cond_wait(&s->cond, &s->lock);
    Pthread_mutex_unlock(&s->lock);
}

void *worker(void *arg)
{
    printf("this should print first\n");
    signal_done(&s);
    return NULL;
}

int main()
{
    pthread_t p;
    signal_init(&s);
    Pthread_create(&p, NULL, worker, NULL);
    signal_wait(&s);
    printf("this should print last\n");
    return 0;
}