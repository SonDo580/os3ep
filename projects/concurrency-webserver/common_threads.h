#ifndef __common_threads_h__
#define __common_threads_h__

#include <pthread.h>
#include <assert.h>

void Pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg)
{
    assert(pthread_create(thread, attr, start_routine, arg) == 0);
}

void Pthread_join(pthread_t thread, void **value_ptr)
{
    assert(pthread_join(thread, value_ptr) == 0);
}

void Pthread_mutex_init(pthread_mutex_t *m, const pthread_mutexattr_t *attr)
{
    assert(pthread_mutex_init(m, attr) == 0);
}

void Pthread_mutex_lock(pthread_mutex_t *m)
{
    assert(pthread_mutex_lock(m) == 0);
}

void Pthread_mutex_unlock(pthread_mutex_t *m)
{
    assert(pthread_mutex_unlock(m) == 0);
}

void Pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr)
{
    assert(pthread_cond_init(cond, attr) == 0);
}

void Pthread_cond_signal(pthread_cond_t *cond)
{
    assert(pthread_cond_signal(cond) == 0);
}

void Pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
    assert(pthread_cond_wait(cond, mutex) == 0);
}

#endif // __common_threads_h__
