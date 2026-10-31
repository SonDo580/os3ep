#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <assert.h>
#include <unistd.h>

#include "comm_lib.h"

// === Client API ===

comm_client_t *comm_client_init(char *server_host, int server_port)
{
    comm_client_t *client = malloc(sizeof(comm_client_t));
    if (client == NULL)
    {
        perror("malloc()");
        return NULL;
    }

    // Create UDP socket
    client->sockfd = socket(AF_INET, SOCK_DGRAM, 0); // IPv4 + UDP
    if (client->sockfd < 0)
    {
        perror("socket()");
        goto fail;
    }

    // Setup destination server address
    client->server_addr.sin_family = AF_INET;
    client->server_addr.sin_port = htons(server_port);

    // Convert hostname/IP to network address
    struct hostent *server = gethostbyname(server_host);
    if (server == NULL)
    {
        fprintf(stderr, "no such host: '%s'\n", server_host);
        goto fail;
    }
    memcpy(&client->server_addr.sin_addr.s_addr, server->h_addr_list[0], server->h_length);

    return client; // success

fail:
    comm_client_close(client);
    return NULL;
}

/* Return number of received bytes if success; -1 if fail. */
ssize_t comm_client_request(comm_client_t *client,
                            void *req, size_t req_len,
                            void *resp, size_t resp_len)
{
    assert(client != NULL && req != NULL && resp != NULL);
    assert(req_len > 0 && req_len <= MAXBUF && resp_len > 0 && resp_len <= MAXBUF);

    // Send request to server
    ssize_t bytes_sent = sendto(client->sockfd, req, req_len, 0,
                                (struct sockaddr *)&client->server_addr, sizeof(client->server_addr));
    if (bytes_sent < 0)
    {
        perror("sendto()");
        return -1;
    }
    else if (bytes_sent == 0)
    {
        fprintf(stderr, "Sent 0 bytes.\n");
        return -1;
    }

    // Read response from server
    struct sockaddr_in from_addr;
    socklen_t from_len = sizeof(from_addr);
    ssize_t bytes_read = recvfrom(client->sockfd, resp, resp_len, 0,
                                  (struct sockaddr *)&from_addr, &from_len);
    if (bytes_read < 0)
    {
        perror("recvfrom()");
        return -1;
    }
    else if (bytes_read == 0)
    {
        fprintf(stderr, "Received empty datagram from server.\n");
        return -1;
    }
    return bytes_read;
}

void comm_client_close(comm_client_t *client)
{
    if (client)
    {
        if (client->sockfd >= 0)
            close(client->sockfd);
        free(client);
    }
}

// === Server API ===

comm_server_t *comm_server_init(int port)
{
    comm_server_t *server = malloc(sizeof(comm_server_t));
    if (server == NULL)
    {
        perror("malloc()");
        goto fail;
    }

    // Create UDP socket
    server->sockfd = socket(AF_INET, SOCK_DGRAM, 0); // IPv4 + UDP
    if (server->sockfd < 0)
    {
        perror("socket()");
        goto fail;
    }

    // Bind
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(server->sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind()");
        goto fail;
    }

    printf("Server listening on port %d\n", port);

    return server; // success

fail:
    comm_server_close(server);
    return NULL;
}

ssize_t comm_server_receive(comm_server_t *server,
                            void *buf, size_t len,
                            comm_endpoint_t *client)
{
    assert(server != NULL && buf != NULL && client != NULL);
    assert(len > 0 && len <= MAXBUF);

    client->client_len = sizeof(client->client_addr);

    ssize_t bytes_read = recvfrom(server->sockfd, buf, len, 0,
                                  (struct sockaddr *)&client->client_addr, &client->client_len);
    if (bytes_read < 0)
        perror("receive from()");
    return bytes_read;
}

ssize_t comm_server_reply(comm_server_t *server,
                          void *buf, size_t len,
                          comm_endpoint_t *client)
{
    assert(server != NULL && buf != NULL && client != NULL);
    assert(len > 0 && len <= MAXBUF);

    ssize_t bytes_sent = sendto(server->sockfd, buf, len, 0,
                                (struct sockaddr *)&client->client_addr, client->client_len);
    if (bytes_sent < 0)
        perror("sendto()");
    return bytes_sent;
}

void comm_server_close(comm_server_t *server)
{
    if (server)
    {
        if (server->sockfd >= 0)
            close(server->sockfd);
        free(server);
    }
}