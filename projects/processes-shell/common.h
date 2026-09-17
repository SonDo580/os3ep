#ifndef __common_h__
#define __common_h__

#include <stddef.h>
#include <stdio.h>

#define ALLOCATE(type, count) \
    (type *)Realloc(NULL, sizeof(type) * (count))

#define GROW_CAPACITY(capacity) \
    ((capacity) == 0 ? 8 : (capacity) * 2)

#define GROW_ARRAY(type, pointer, new_count) \
    (type *)Realloc(pointer, sizeof(type) * (new_count))

void print_err();
void panic();
void *Realloc(void *pointer, size_t new_size);
FILE *Fopen(const char *filename, const char *modes);

#endif
