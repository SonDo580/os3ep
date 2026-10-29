#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "common.h"

void print_usage(const char *prog_name)
{
    fprintf(stderr, "usage: %s [-h server_host] [-p server_port] [-m message]\n", prog_name);
}

int main(int argc, char *argv[])
{
    char *server_host = "localhost";
    int server_port = 8000;
    char *message = "hello";

    int sd = -1;
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

    // Create UDP socket
    sd = socket(AF_INET, SOCK_DGRAM, 0); // IPv4 + UDP
    if (sd < 0)
    {
        perror("socket()");
        goto cleanup;
    }

    // Setup destination server address
    struct sockaddr_in server_addr = {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);

    // Convert hostname/IP to network address
    struct hostent *server = gethostbyname(server_host);
    if (server == NULL)
    {
        fprintf(stderr, "no such host: '%s'\n", server_host);
        goto cleanup;
    }
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr_list[0], server->h_length);

    // Send message to server
    ssize_t bytes_sent = sendto(sd, message, strlen(message), 0,
                                (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (bytes_sent < 0)
    {
        perror("sendto()");
        goto cleanup;
    }
    else if (bytes_sent == 0)
    {
        fprintf(stderr, "Sent 0 bytes.\n");
        goto cleanup;
    }

    struct sockaddr_in from_addr;
    socklen_t from_len = sizeof(from_addr);

    // Read response from server
    char reply[BUFFER_SIZE];
    ssize_t bytes_read = recvfrom(sd, reply, BUFFER_SIZE - 1, 0,
                                  (struct sockaddr *)&from_addr, &from_len);

    if (bytes_read < 0)
    {
        perror("recvfrom()");
        goto cleanup;
    }
    else if (bytes_read == 0)
    {
        fprintf(stderr, "Received empty datagram from server.\n");
        goto cleanup;
    }

    reply[bytes_read] = '\0';
    printf("Server reply: %s\n", reply);

    exit_code = EXIT_SUCCESS;

cleanup:
    if (sd >= 0)
        close(sd);
    return exit_code;
}