// Prints out:
// - location of main() - where code lives
// - location of a heap-allocated value
// - location of a value on stack

#include <stdio.h>
#include <stdlib.h>

int main()
{
    printf("location in code : %p\n", main);
    printf("location in heap : %p\n", malloc(100e6));
    int x = 3;
    printf("location in stack : %p\n", &x);
    return 0;
}

// Example result:
// location in code : 0x55acf161f189
// location in heap : 0x7efd598a1010
// location in stack : 0x7fffd7ded194
//
// Note:
// - all of these addresses are virtual, and will be translated
//   by the OS and hardware to physical locations.