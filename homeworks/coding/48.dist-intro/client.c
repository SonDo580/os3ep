#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "comm_lib.h"

void print_usage(const char *prog_name)
{
    fprintf(stderr, "usage: %s [-h server_host] [-p server_port] [-m message]\n", prog_name);
}

int main(int argc, char *argv[])
{
    char *server_host = "localhost";
    int server_port = 8000;
    char *message = "hello";

    comm_client_t *client = NULL;
    int exit_code = EXIT_FAILURE;

    // Parse command-line arguments
    int opt;
    while ((opt = getopt(argc, argv, "h:p:m:")) != -1)
    {
        switch (opt)
        {
        case 'h':
            server_host = optarg;
            break;
        case 'p':
            server_port = atoi(optarg);
            break;
        case 'm':
            message = optarg;
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

    client = comm_client_init(server_host, server_port);
    if (client == NULL)
    {
        perror("comm_client_init()");
        goto cleanup;
    }

    // Send request and wait for response
    char reply[MAXBUF];
    ssize_t bytes_received = comm_client_request(client, message, strlen(message), reply, sizeof(reply) - 1);
    if (bytes_received < 0)
    {
        perror("comm_client_request()");
        goto cleanup;
    }

    reply[bytes_received] = '\0';
    printf("Server reply: %s\n", reply);

    exit_code = EXIT_SUCCESS;

cleanup:
    comm_client_close(client);
    return exit_code;
}