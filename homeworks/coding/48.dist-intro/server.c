#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "common.h"

void print_usage(const char *prog_name)
{
    fprintf(stderr, "usage: %s [-p port]\n", prog_name);
}

int main(int argc, char *argv[])
{
    int port = 8000;

    int sd = -1;
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

    // Create UDP socket
    sd = socket(AF_INET, SOCK_DGRAM, 0); // IPv4 + UDP
    if (sd < 0)
    {
        perror("socket()");
        goto cleanup;
    }

    // Bind
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind()");
        goto cleanup;
    }

    printf("Server listening on port %d\n", port);

    char message[BUFFER_SIZE];

    while (1)
    {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        // Receive message from client
        ssize_t bytes_read = recvfrom(sd, message, BUFFER_SIZE - 1, 0,
                                      (struct sockaddr *)&client_addr, &client_len);

        if (bytes_read < 0)
        {
            perror("receive from()");
            continue;
        }
        else if (bytes_read == 0)
            continue;

        message[bytes_read] = '\0';
        printf("Received: %s\n", message);

        char *reply = "ok"; // ack
        ssize_t bytes_sent = sendto(sd, reply, strlen(reply), 0,
                                    (struct sockaddr *)&client_addr, client_len);
        if (bytes_sent < 0)
            perror("sendto()");
    }

    exit_code = EXIT_SUCCESS;

cleanup:
    if (sd >= 0)
        close(sd);
    return exit_code;
}
