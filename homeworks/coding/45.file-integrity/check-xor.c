/*
Computes an XOR-based checksum over an input file,
and prints the checksum as output.
Use a 8-bit unsigned char to store the (one byte) checksum.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define BLOCK_SIZE 4096 // 4KB

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "usage: %s <input_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *filename = argv[1];

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

    // 0 XOR a = a -> use 0 as initial value
    unsigned char checksum = 0;

    unsigned char buf[BLOCK_SIZE];
    int bytes_read;

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
        else if (bytes_read == 0) // EOF
            break;

        // compute checksum over 1-byte chunks
        for (int i = 0; i < bytes_read; i++)
            checksum ^= buf[i];
    }

    // print checksum (MSB to LSB)
    for (int i = 7; i >= 0; i--)
    {
        int bit = (checksum >> i) & 1;
        printf("%d", bit);
    }
    printf("\n");

    return 0;
}