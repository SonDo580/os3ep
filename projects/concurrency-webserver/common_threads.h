#ifndef __common_threads_h__
#define __common_threads_h__

#include <pthread.h>

void Pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg);
void Pthread_join(pthread_t thread, void **value_ptr);

void Pthread_mutex_init(pthread_mutex_t *m, const pthread_mutexattr_t *attr);
void Pthread_mutex_lock(pthread_mutex_t *m);
void Pthread_mutex_unlock(pthread_mutex_t *m);

void Pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr);
void Pthread_cond_signal(pthread_cond_t *cond);
void Pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);

#endif
