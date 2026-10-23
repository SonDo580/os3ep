/*
Tail:

Write a program that prints out the last few lines of a file.

The program should be efficient, in that it seeks to near the end of the file,
reads in a block of data, and then goes backwards until it finds the requested number of lines;
at this point, it should print out those lines from beginning to the end of the file.

To invoke the program, one should type: 'mytail -n file',
where n is the number of lines at the end of the file to print.

Useful interfaces: stat(), lseek(), open(), read(), close().
*/

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#define BLOCK_SIZE 4096 // 4KB

int main(int argc, char *argv[])
{
    if (argc != 3 || argv[1][0] != '-')
    {
        fprintf(stderr, "usage: %s -<n> <file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int target_lines = atoi(&(argv[1][1]));
    if (target_lines <= 0)
    {
        fprintf(stderr, "n must be > 0\n");
        exit(EXIT_FAILURE);
    }

    char *filename = argv[2];

    struct stat sbuf;
    if (stat(filename, &sbuf) < 0)
    {
        perror("fstat()");
        exit(EXIT_FAILURE);
    }
    if (!S_ISREG(sbuf.st_mode))
    {
        fprintf(stderr, "error: %s is not a regular file\n", filename);
        exit(EXIT_FAILURE);
    }

    int fd = open(filename, O_RDONLY);
    if (fd < 0)
    {
        perror("open()");
        exit(EXIT_FAILURE);
    }

    // === Find starting offset of the last n lines ===
    // - Seek to near the end of the file.
    // - Read a block into a buffer.
    // - Scan backward through the buffer looking for '\n'.
    // - Repeat until found enough lines or reached file start.

    off_t file_size = lseek(fd, 0, SEEK_END);
    off_t current_pos = file_size;
    int found_end_lines = 0; // number of '\n' found (backward)
    off_t target_start = 0;  // start of last n lines
    char buf[BLOCK_SIZE];
    ssize_t bytes_read;

    while (current_pos > 0 && found_end_lines < target_lines)
    {
        // How much to step back
        off_t read_size = current_pos < BLOCK_SIZE ? current_pos : BLOCK_SIZE;
        current_pos -= read_size;

        // Step back and read the block
        lseek(fd, current_pos, SEEK_SET);

        do
            bytes_read = read(fd, buf, read_size);
        while (bytes_read < 0 && errno == EINTR);

        if (bytes_read < 0)
        {
            perror("read()");
            close(fd);
            exit(EXIT_FAILURE);
        }

        // Scan backward through the buffer looking for '\n'
        for (ssize_t i = bytes_read - 1; i >= 0; i--)
        {
            if (buf[i] == '\n')
            {
                found_end_lines++;
                if (found_end_lines == target_lines)
                {
                    target_start = current_pos + i + 1;
                    break;
                }
            }
        }
    }

    // === Print the last n lines ===

    lseek(fd, target_start, SEEK_SET);

    while (1)
    {
        do
            bytes_read = read(fd, buf, sizeof(buf));
        while (bytes_read < 0 && errno == EINTR);

        if (bytes_read < 0)
        {
            perror("read()");
            close(fd);
            exit(EXIT_FAILURE);
        }

        if (bytes_read == 0) // EOF
            break;

        size_t bytes_written = fwrite(buf, sizeof(char), bytes_read, stdout);
        if (bytes_written < (size_t)bytes_read)
        {
            perror("fwrite()");
            close(fd);
            exit(EXIT_FAILURE);
        }
    }

    close(fd);
    return 0;
}