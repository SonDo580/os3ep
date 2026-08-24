# 1: First, write a simple program called null.c that creates a pointer to an integer, sets it to NULL, and then tries to dereference it. Compile this into an executable called null. What happens when you run this program?

- Program: `1.null.c`

# 2: Run the program under the debugger by typing `gdb ./null` and then, once `gdb` is running, typing `run`. What does gdb show you?

```
Program received signal SIGSEGV, Segmentation fault.
0x000055555555519f in main () at null.c:16
16          printf("x = %d\n", *px); // -> Segmentation fault (core dumped)
```

# 3: Finally, use the `valgrind` tool on this program. We’ll use `memcheck` that is a part of `valgrind` to analyze what happens. Run this by typing in the following: `valgrind --leak-check=yes ./null`. What happens when you run this? Can you interpret the output from the tool?

```
...
==9525== Command: ./null
==9525==
==9525== Invalid read of size 4
==9525==    at 0x10919F: main (null.c:16)
==9525==  Address 0x0 is not stack'd, malloc'd or (recently) free'd
==9525==
==9525==
==9525== Process terminating with default action of signal 11 (SIGSEGV)
==9525==  Access not within mapped region at address 0x0
==9525==    at 0x10919F: main (null.c:16)
...
Segmentation fault (core dumped)
```

- The output shows that we are trying to read an invalid address (0x0), which is neither in stack nor heap.

# 4: Write a simple program that allocates memory using `malloc()` but forgets to free it before exiting. What happens when program runs? Can you use `gdb` to find any problems with it? How about `valgrind` (again with the `--leak-check=yes` flag)?

- Program: see `4.c`.
- What happens: it runs "normally".
- `gdb`: shows that process exited normally
- `valgrind`: shows that **memory leaked** detected

```
==10401== HEAP SUMMARY:
==10401==     in use at exit: 10 bytes in 1 blocks
==10401==   total heap usage: 1 allocs, 0 frees, 10 bytes allocated
==10401==
==10401== 10 bytes in 1 blocks are definitely lost in loss record 1 of 1
...
==10401== LEAK SUMMARY:
==10401==    definitely lost: 10 bytes in 1 blocks
==10401==    indirectly lost: 0 bytes in 0 blocks
==10401==      possibly lost: 0 bytes in 0 blocks
==10401==    still reachable: 0 bytes in 0 blocks
==10401==         suppressed: 0 bytes in 0 blocks
```

# 6: Create a program that allocates an array of integers (as above), frees them, and then tries to print the value of one of the elements of the array. Does the program run? What happens when you use `valgrind` on it?

- Program: `6.c`.
- The program still runs, but print out an "unknown" character (not the one set before `free()`).
- `valgrind`: show that we are trying to read an invalid address (freed)

```
...
==11553== Invalid read of size 1
==11553==    at 0x109221: main (6.c:21)
==11553==  Address 0x4a9a041 is 1 bytes inside a block of size 10 free'd
...
```

# 7: Now pass a funny value to free (e.g., a pointer in the middle of the array you allocated above). What happens? Do you need tools to find this type of problem?

- The compiler will "yells" at us.

# 8: Try out some of the other interfaces to memory allocation. For example, create a simple vector-like data structure and related routines that use `realloc()` to manage the vector. Use an array to store the vectors elements; when a user adds an entry to the vector, use `realloc()` to allocate more space for it. How well does such a vector perform? How does it compare to a linked list? Use `valgrind` to help you find bugs

- Program: `8.vector.c`
