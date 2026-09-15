#include "common.h"

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

FILE *Fopen(const char *filename, const char *modes)
{
    FILE *fp = fopen(filename, modes);
    if (fp == NULL)
    {
        fprintf(stderr, "cannot open file '%s'\n", filename);
        exit(1);
    }
    return fp;
}