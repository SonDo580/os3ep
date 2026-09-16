#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include "command.h"

const char *err_msg = "An error has occurred\n";

void print_err()
{
    write(STDERR_FILENO, err_msg, strlen(err_msg));
}

void panic()
{
    print_err();
    exit(EXIT_FAILURE);
}

void *Realloc(void *pointer, size_t new_size)
{
    if (new_size == 0)
    {
        free(pointer);
        return NULL;
    }

    void *result = realloc(pointer, new_size);
    if (result == NULL)
        panic();
    return result;
}

FILE *Fopen(const char *filename, const char *modes)
{
    FILE *fp = fopen(filename, modes);
    if (fp == NULL)
        panic();
    return fp;
}