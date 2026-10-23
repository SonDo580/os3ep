/*
Stat:

Write your own version of the command line program 'stat',
which simply calls the 'stat()' system call on a given file or directory.

Print out file size, number of blocks allocated, reference (link) count, and so forth.

What is the link count of a directory,
as the number of entries in the directory changes?
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "usage: %s <file or directory>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *name = argv[1];
    struct stat sbuf;

    int rc = stat(name, &sbuf);
    if (rc < 0)
    {
        perror("stat()");
        exit(EXIT_FAILURE);
    }

    printf("Inode number: %lu\n", (unsigned long)sbuf.st_ino);
    printf("File size (bytes): %lld\n", (long long)sbuf.st_size);
    printf("Number of (512-byte) blocks: %lld\n", (long long)sbuf.st_blocks);
    printf("Reference count: %lu\n", (unsigned long)sbuf.st_nlink);
    // ... more

    return 0;
}

/*
- For an empty directory, the reference count is 2 (. and ..).
- Creating and removing files inside the directory don't change the reference count.
  But creating and removing directories do.
*/