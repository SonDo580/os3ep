#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

int main()
{
    char *filename = "./2.output";

    int rc = fork();
    if (rc < 0) // fork failed
    {
        fprintf(stderr, "fork failed\n");
        exit(EXIT_FAILURE);
    }
    else if (rc == 0) // child (new) process
    {
        int fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
        if (fd < 0)
        {
            fprintf(stderr, "can not open %s\n", filename);
            exit(EXIT_FAILURE);
        }
        printf("(child) fd: %d\n", fd);

        char *text = "Hello, I'm child\n";
        if (write(fd, text, strlen(text)) < 0)
        {
            fprintf(stderr, "(child) write failed\n");
            close(fd);
            exit(EXIT_FAILURE);
        }
    }
    else // parent goes down this path
    {
        int fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
        if (fd < 0)
        {
            fprintf(stderr, "can not open %s\n", filename);
            exit(EXIT_FAILURE);
        }
        printf("(parent) fd: %d\n", fd);

        char *text = "Hello, I'm parent\n";
        if (write(fd, text, strlen(text)) < 0)
        {
            fprintf(stderr, "(parent) write failed\n");
            close(fd);
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}

/*
- each process has its own file offset,
  so writes may overwrite each other.
*/