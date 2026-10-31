#ifndef __comm_lib_h__
#define __comm_lib_h__

#include <netinet/ip.h>

#define MAXBUF 8192

typedef struct
{
    int sockfd;
    struct sockaddr_in server_addr;
} comm_client_t;

typedef struct
{
    int sockfd;
} comm_server_t;

// Track UDP client on server
typedef struct
{
    struct sockaddr_in client_addr;
    socklen_t client_len;
} comm_endpoint_t;

// === Client API ===
comm_client_t *comm_client_init(char *server_host, int server_port);
ssize_t comm_client_request(comm_client_t *client,
                            void *req, size_t req_len,
                            void *resp, size_t resp_len);
void comm_client_close(comm_client_t *client);

// === Server API ===
comm_server_t *comm_server_init(int port);
ssize_t comm_server_receive(comm_server_t *server,
                            void *buf, size_t len,
                            comm_endpoint_t *client);
ssize_t comm_server_reply(comm_server_t *server,
                          void *buf, size_t len,
                          comm_endpoint_t *client);
void comm_server_close(comm_server_t *server);

#endif
