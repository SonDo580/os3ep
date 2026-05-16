/*
Write a simple program called null.c that creates a pointer to an integer,
sets it to NULL, and then tries to dereference it.
Compile this into an executable called null.
What happens when you run this program?
*/

#include <stddef.h>
#include <stdio.h>

int main()
{
    int x = 10;
    int *px = &x;
    px = NULL;
    printf("x = %d\n", *px); // -> Segmentation fault (core dumped)
    return 0;
}
