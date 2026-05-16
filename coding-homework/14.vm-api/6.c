/*
Create a program that allocates an array of integers (as above), frees them,
and then tries to print the value of one of the elements of the array
*/

#include <stdlib.h>
#include <stdio.h>

int main()
{
    size_t size = 10;
    char *bytes = (char *)malloc(size);
    for (int i = 0; i < size; i++)
    {
        bytes[i] = 'a';
    }

    int i = 1;
    printf("bytes[%d] before free: %c\n", i, bytes[i]);
    // free(bytes + 1); // -> compile error
    free(bytes);
    printf("bytes[%d] after free: %c\n", i, bytes[i]);

    return 0;
}