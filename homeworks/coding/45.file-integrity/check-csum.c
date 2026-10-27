/*
'check-csum':
- Reads a file, computes the checksum, compares the result with stored checksum.
- If there's a problem, prints that the file has been corrupted.
- Related tool: 'create-csum'.

Implementation: use XOR-based checksum (match 'create-csum').
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
    int exit_code = EXIT_FAILURE;
    int in_fd, check_fd;

    if (argc != 3)
    {
        fprintf(stderr, "usage: %s <input_file> <checksum_file>\n", argv[0]);
        goto cleanup;
    }

    char *in_filename = argv[1];
    char *check_filename = argv[2];

    struct stat in_sbuf;
    if (stat(in_filename, &in_sbuf) < 0)
    {
        fprintf(stderr, "%s: ", in_filename);
        perror("fstat()");
        goto cleanup;
    }
    if (!S_ISREG(in_sbuf.st_mode))
    {
        fprintf(stderr, "error: %s is not a regular file\n", in_filename);
        goto cleanup;
    }

    struct stat check_sbuf;
    if (stat(check_filename, &check_sbuf) < 0)
    {
        fprintf(stderr, "%s: ", check_filename);
        perror("fstat()");
        goto cleanup;
    }
    if (!S_ISREG(check_sbuf.st_mode))
    {
        fprintf(stderr, "error: %s is not a regular file\n", check_filename);
        goto cleanup;
    }

    in_fd = open(in_filename, O_RDONLY);
    if (in_fd < 0)
    {
        fprintf(stderr, "%s: ", in_filename);
        perror("open()");
        goto cleanup;
    }

    check_fd = open(check_filename, O_RDONLY);
    if (check_fd < 0)
    {
        fprintf(stderr, "%s: ", check_filename);
        perror("open()");
        goto cleanup;
    }

    unsigned char in_read_buf[BLOCK_SIZE];
    ssize_t in_bytes_read;

    // Read data from checksum file on demand
    unsigned char check_read_buf[BLOCK_SIZE];
    ssize_t check_bytes_read = 0;
    ssize_t check_bytes_consume = 0;

    while (1)
    {
        do
            in_bytes_read = read(in_fd, in_read_buf, sizeof(in_read_buf));
        while (in_bytes_read < 0 && errno == EINTR);

        if (in_bytes_read < 0)
        {
            fprintf(stderr, "%s: ", in_filename);
            perror("read()");
            goto cleanup;
        }
        else if (in_bytes_read == 0) // EOF
            break;

        // compute checksum over 1-byte chunks of read data
        unsigned char computed_block_checksum = 0;
        for (int i = 0; i < in_bytes_read; i++)
            computed_block_checksum ^= in_read_buf[i];

        // fetch extra bytes from checksum file if:
        // - no bytes have been fetched.
        // - OR all previously fetched bytes have been consumed.
        if (check_bytes_consume == check_bytes_read)
        {
            do
                check_bytes_read = read(check_fd, check_read_buf, sizeof(check_read_buf));
            while (check_bytes_read < 0 && errno == EINTR);

            if (check_bytes_read < 0)
            {
                fprintf(stderr, "%s: ", check_filename);
                perror("read()");
                goto cleanup;
            }
            else if (check_bytes_read == 0) // EOF
            {
                // no more stored checksums; input still remains
                // -> not match
                fprintf(stderr, "file '%s' corrupted\n", in_filename);
                goto cleanup;
            }

            // start consuming from check_read_buf[0]
            check_bytes_consume = 0; 
        }

        // compare computed checksum with stored checksum
        if (computed_block_checksum != check_read_buf[check_bytes_consume])
        {
            fprintf(stderr, "file '%s' corrupted\n", in_filename);
            goto cleanup;
        }
        else
            check_bytes_consume++;
    }

    // input file reached EOF; stored checksums still remain
    // -> not match

    if (check_bytes_consume < check_bytes_read)
    { // buffer hasn't been fully consumed
        fprintf(stderr, "file '%s' corrupted\n", in_filename);
        goto cleanup;
    }
    else if (check_bytes_consume == check_bytes_read)
    { // try reading checksum file 1 more time
        do
            check_bytes_read = read(check_fd, check_read_buf, sizeof(check_read_buf));
        while (check_bytes_read < 0 && errno == EINTR);

        if (check_bytes_read < 0)
        {
            fprintf(stderr, "%s: ", check_filename);
            perror("read()");
            goto cleanup;
        }
        else if (check_bytes_read > 0)
        {
            fprintf(stderr, "file '%s' corrupted\n", in_filename);
            goto cleanup;
        }
    }

    exit_code = EXIT_SUCCESS;

cleanup:
    if (in_fd >= 0)
        close(in_fd);
    if (check_fd >= 0)
        close(check_fd);

    return exit_code;
}