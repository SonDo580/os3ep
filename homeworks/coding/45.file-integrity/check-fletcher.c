/*
Computes the Fletcher checksum over an input file.

- Assume a block D consists of bytes `d1...dn`:
  . initially: `s1 = s2 = 0`.
  . `s1 = (s1 + di) mod 255` over all di.
  . `s2 = (s2 + s1) mod 255` over all di.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define BLOCK_SIZE 4096 // 4KB

void print_bits(unsigned char s, char *name)
{
    printf("%s: ", name);
    for (int i = 7; i >= 0; i--)
    {
        int bit = (s >> i) & 1;
        printf("%d", bit);
    }
    printf("\n");
}

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

    // Although the values stay within 0 to 254 after the modulo operation,
    // the formula for s2 involves (s2 + s1), which will overflow
    // if we store the values as 'unsigned char'
    // -> Use 'unsigned int'.
    unsigned int s1 = 0;
    unsigned int s2 = 0;

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

        // compute 2 check bytes over 1-byte chunks
        for (int i = 0; i < bytes_read; i++)
        {
            s1 = (s1 + buf[i]) % 255;
            s2 = (s2 + s1) % 255;
        }
    }

    // print 2 check bytes (MSB to LSB)
    print_bits((unsigned char)s1, "s1");
    print_bits((unsigned char)s2, "s2");

    close(fd);
    return 0;
}