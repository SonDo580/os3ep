#include <stdlib.h>
#include <assert.h>

#include "common.h"

void *Malloc(size_t nbytes)
{
    void *ptr = malloc(nbytes);
    assert(ptr != NULL);
    return ptr;
}
