#ifndef __common_h__
#define __common_h__

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

void *Malloc(size_t size)
{
    assert(size > 0);
    void *ptr = malloc(size);
    if (ptr == NULL)
    {
        fprintf(stderr, "malloc failed\n");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

void *Calloc(size_t nmemb, size_t size)
{
    assert(nmemb > 0 && size > 0);
    void *ptr = calloc(nmemb, size);
    if (ptr == NULL)
    {
        fprintf(stderr, "calloc failed\n");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

#endif