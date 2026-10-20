#ifndef __common_h__
#define __common_h__

#include <stddef.h>

#define MAX_QUEUE_SIZE 1024

#define SchedAlgoFIFO "FIFO"
#define SchedAlgoSFF "SFF"

void *Malloc(size_t nbytes);

#endif