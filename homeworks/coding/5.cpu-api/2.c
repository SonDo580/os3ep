/*
Write a program that opens a file (with the open() system call)
and then calls fork() to create a new process.
Can both the child and parent access the file descriptor returned by open()?
What happens when they are writing to the file concurrently,
i.e., at the same time?
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

int main()
{
    char *filename = "./2.output";
    int fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
    if (fd < 0)
    {
        fprintf(stderr, "can not open %s\n", filename);
        exit(EXIT_FAILURE);
    }

    int rc = fork();
    if (rc < 0) // fork failed
    {
        fprintf(stderr, "fork failed\n");
        close(fd);
        exit(EXIT_FAILURE);
    }
    else if (rc == 0) // child (new) process
    {
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
        printf("(parent) fd: %d\n", fd);

        char *text = "Hello, I'm parent\n";
        if (write(fd, text, strlen(text)) < 0)
        {
            fprintf(stderr, "(parent) write failed\n");
            close(fd);
            exit(EXIT_FAILURE);
        }
    }

    close(fd);
    return 0;
}

/*
- both child and parent can access 'fd'
- concurrent writes does not overwrite each other,
  since child and parent share the same file offset
  (each starts from where the other left off)

- overwriting happens if each process opens the file separately
  (see 2.alt.c) 
*/