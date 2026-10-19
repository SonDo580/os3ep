//
// client.c: A very, very primitive HTTP client.
//
// To run, try:
//      client hostname portnumber filename
//
// Sends one HTTP request to the specified HTTP server.
// Prints out the HTTP response.
//
// For testing your server, you will want to modify this client.
// For example:
// You may want to make this multi-threaded so that you can
// send many requests simultaneously to the server.
//
// You may also want to be able to request different URIs;
// you may want to get more URIs from the command line
// or read the list from a file.
//
// When we test your server, we will be using modifications to this client.
//

#include <unistd.h>
#include <assert.h>

#include "io_helper.h"
#include "common_threads.h"

#define MAXBUF (8192)
#define MAX_THREADS 100

//
// Send an HTTP request for the specified file
//
void client_send(int fd, char *filename)
{
    char buf[MAXBUF];
    char hostname[MAXBUF];

    gethostname_or_die(hostname, MAXBUF);

    /* Form and send the HTTP request */
    sprintf(buf, "GET %s HTTP/1.1\n", filename);
    sprintf(buf, "%shost: %s\n\r\n", buf, hostname);
    write_or_die(fd, buf, strlen(buf));
}

//
// Read the HTTP response and print it out
//
void client_print(int fd)
{
    char buf[MAXBUF];
    int n;

    // Read and display the HTTP Header
    n = readline_or_die(fd, buf, MAXBUF);
    while (strcmp(buf, "\r\n") && (n > 0))
    {
        printf("Header: %s", buf);
        n = readline_or_die(fd, buf, MAXBUF);

        // If you want to look for certain HTTP tags...
        // int length = 0;
        // if (sscanf(buf, "Content-Length: %d ", &length) == 1) {
        //    printf("Length = %d\n", length);
        //}
    }

    // Read and display the HTTP Body
    n = readline_or_die(fd, buf, MAXBUF);
    while (n > 0)
    {
        printf("%s", buf);
        n = readline_or_die(fd, buf, MAXBUF);
    }
}

typedef struct __Arg
{
    char *filename;
    char *host;
    int port;
} Arg;

void *worker(void *__arg)
{
    Arg *arg = (Arg *)__arg;

    int clientfd = open_client_fd_or_die(arg->host, arg->port);

    client_send(clientfd, arg->filename);
    client_print(clientfd);

    close_or_die(clientfd);
}

char *usage_msg = "usage: wclient <host> <port> <filename> [-t <threads>]\n";

int main(int argc, char *argv[])
{
    char *host, *filename;
    int port;
    int num_threads = 1;
    int clientfd;

    int opt;
    while ((opt = getopt(argc, argv, "t:")) != -1)
    {
        switch (opt)
        {
        case 't':
            num_threads = atoi(optarg);
            assert(num_threads > 0 && num_threads <= MAX_THREADS);
            break;
        default:
            fprintf(stderr, usage_msg);
            exit(1);
        }
    }

    if (argc - optind != 3)
    {
        fprintf(stderr, usage_msg);
        exit(1);
    }
    host = argv[optind];
    port = atoi(argv[optind + 1]);
    filename = argv[optind + 2];

    Arg arg = {
        .host = host,
        .port = port,
        .filename = filename,
    };

    pthread_t pid[num_threads];
    for (int i = 0; i < num_threads; i++)
        Pthread_create(&pid[i], NULL, worker, (void *)&arg);

    for (int i = 0; i < num_threads; i++)
        Pthread_join(pid[i], NULL);

    exit(0);
}
