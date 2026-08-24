/*
Write a simple program that allocates memory using malloc()
but forgets to free it before exiting.
*/

#include <stdlib.h>

int main()
{
    size_t size = 10;
    char *bytes = (char *)malloc(size);
    for (int i = 0; i < size; i++)
    {
        bytes[i] = 0;
    }
    // don't free
    return 0;
}