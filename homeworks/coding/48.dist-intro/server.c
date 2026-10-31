#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "comm_lib.h"

void print_usage(const char *prog_name)
{
    fprintf(stderr, "usage: %s [-p port]\n", prog_name);
}

int main(int argc, char *argv[])
{
    int port = 8000;

    comm_server_t *server = NULL;
    int exit_code = EXIT_FAILURE;

    // Parse command-line arguments
    int opt;
    while ((opt = getopt(argc, argv, "p:")) != -1)
    {
        switch (opt)
        {
        case 'p':
            port = atoi(optarg);
            break;
        default:
            print_usage(argv[0]);
            goto cleanup;
        }
    }
    if (optind != argc)
    {
        print_usage(argv[0]);
        goto cleanup;
    }

    server = comm_server_init(port);
    if (server == NULL)
    {
        perror("comm_server_init()");
        goto cleanup;
    }

    char req[MAXBUF];

    while (1)
    {
        comm_endpoint_t client;

        // Receive message from client
        ssize_t bytes_read = comm_server_receive(server, req, sizeof(req) - 1, &client);
        if (bytes_read <= 0)
        {
            perror("comm_server_receive()");
            continue;
        }
        else if (bytes_read == 0)
            continue;

        req[bytes_read] = '\0';
        printf("Received: %s\n", req);

        char *reply = "ok"; // ack
        ssize_t bytes_sent = comm_server_reply(server, reply, strlen(reply), &client);
        if (bytes_sent < 0)
            perror("comm_server_reply()");
    }

    exit_code = EXIT_SUCCESS;

cleanup:
    comm_server_close(server);
    return exit_code;
}
