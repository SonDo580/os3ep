/*
'create-csum':
- Computes a single-byte checksum for every 4KB block of a file.
- Records the result in an output file.
- Related tool: 'check-csum'

Implementation: use XOR-based checksum for simplicity.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define BLOCK_SIZE 4096 // 4KB

/* Return 0 on success; -1 on failure. */
int write_all(int fd, unsigned char *buf, size_t size)
{
    size_t total_bytes_written = 0;
    while (total_bytes_written < size)
    {
        ssize_t bytes_written = write(fd, buf + total_bytes_written, size - total_bytes_written);
        if (bytes_written < 0)
        {
            if (errno == EINTR)
                continue; // Retry on interrupt
            else
                return -1; // Error
        }

        total_bytes_written += bytes_written;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    int exit_code = EXIT_FAILURE;
    int in_fd, out_fd;

    if (argc != 3)
    {
        fprintf(stderr, "usage: %s <input_file> <output_file>\n", argv[0]);
        goto cleanup;
    }

    char *in_filename = argv[1];
    char *out_filename = argv[2];

    struct stat sbuf;
    if (stat(in_filename, &sbuf) < 0)
    {
        fprintf(stderr, "%s: ", in_filename);
        perror("fstat()");
        goto cleanup;
    }
    if (!S_ISREG(sbuf.st_mode))
    {
        fprintf(stderr, "error: %s is not a regular file\n", in_filename);
        goto cleanup;
    }

    in_fd = open(in_filename, O_RDONLY);
    if (in_fd < 0)
    {
        fprintf(stderr, "%s: ", in_filename);
        perror("open()");
        goto cleanup;
    }

    out_fd = open(out_filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out_fd < 0)
    {
        fprintf(stderr, "%s: ", out_filename);
        perror("open()");
        goto cleanup;
    }

    unsigned char read_buf[BLOCK_SIZE];
    ssize_t bytes_read;

    unsigned char write_buf[BLOCK_SIZE];
    size_t write_buf_index = 0;

    while (1)
    {
        unsigned char block_checksum = 0;

        do
            bytes_read = read(in_fd, read_buf, sizeof(read_buf));
        while (bytes_read < 0 && errno == EINTR);

        if (bytes_read < 0)
        {
            perror("read()");
            goto cleanup;
        }
        else if (bytes_read == 0) // EOF
            break;

        // compute checksum over 1-byte chunks
        for (int i = 0; i < bytes_read; i++)
            block_checksum ^= read_buf[i];

        // buffer block checksum write (app level)
        // - Even though the OS buffers writes in page cache,
        //   calling write() system call for every byte is still costly.
        write_buf[write_buf_index] = block_checksum;
        write_buf_index++;

        // write block checksums to output file when buffer reach limit
        if (write_buf_index == BLOCK_SIZE)
        {
            if (write_all(out_fd, write_buf, write_buf_index) < 0)
            {
                perror("write()");
                goto cleanup;
            }

            write_buf_index = 0;
        }
    }

    // write remaining block checksums to output file
    if (write_buf_index > 0)
    {
        if (write_all(out_fd, write_buf, write_buf_index) < 0)
        {
            perror("write()");
            goto cleanup;
        }
    }

    exit_code = EXIT_SUCCESS;

cleanup:
    if (in_fd >= 0)
        close(in_fd);
    if (out_fd >= 0)
        close(out_fd);

    return exit_code;
}