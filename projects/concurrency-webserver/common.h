#ifndef __common_h__
#define __common_h__

#include <stdlib.h>
#include <assert.h>

void *Malloc(size_t nbytes)
{
    void *ptr = malloc(nbytes);
    assert(ptr != NULL);
    return ptr;
}

#endif