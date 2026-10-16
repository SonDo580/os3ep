/*
Rendezvous problem:
- You have 2 threads, each of which is about to enter the rendezvous point in the code.
- Neither should exit this part of the code before the other enters it.

Expected result:
- Each child should print their "before" message
  before either prints their "after" message.
*/

#include <stdio.h>
#include <unistd.h>

#include "common_threads.h"

sem_t s1, s2;

void *child_1(void *arg)
{
    sleep(1);
    printf("child 1: before\n");

    // Signal to child 2 that child 1 has entered rendezvous section
    // -> Should initialize s1's value to 0
    //    . If child 2 is waiting, increment s1's value from -1 to 0.
    //    . Otherwise, increment s1's value from 0 to 1.
    //      When child 2 call sem_wait() on s1, it returns right away.
    Sem_post(&s1);

    // Only proceed if child 2 has entered rendezvous section
    // -> Should initialize s2's value to 0
    //    so sem_wait() decrements it to -1 and blocks child 1.
    Sem_wait(&s2);

    printf("child 1: after\n");
    return NULL;
}

void *child_2(void *arg)
{
    sleep(1);
    printf("child 2: before\n");

    // Similar reasoning as child 1
    Sem_post(&s2);
    Sem_wait(&s1);

    printf("child 2: after\n");
    return NULL;
}

int main()
{
    pthread_t p1, p2;
    printf("parent: begin\n");

    // Initialize values to 0 (see reasons in child_1())
    Sem_init(&s1, 0, 0);
    Sem_init(&s2, 0, 0);

    Pthread_create(&p1, NULL, child_1, NULL);
    Pthread_create(&p2, NULL, child_2, NULL);

    Pthread_join(p1, NULL);
    Pthread_join(p2, NULL);

    printf("parent: end\n");
    return 0;
}