#ifndef __common_h__
#define __common_h__

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

void *Malloc(size_t size);
void *Calloc(size_t nmemb, size_t size);
FILE *Fopen(const char *filename, const char *modes);

#endif