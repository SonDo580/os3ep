#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <time.h>

// Pool of connected descriptors
typedef struct
{
    int maxfd;        // largest descriptor in read_set
    fd_set read_set;  // set of active descriptors (including listenfd)
    fd_set ready_set; // subset of descriptors ready for reading
    int nready;       // number of ready descriptors
    int maxi;         // high watermark index into clientfd array
    int clientfd[FD_SETSIZE];
} pool;

void init_pool(int listenfd, pool *p);
void add_client(int connfd, struct sockaddr_in client_addr, pool *p);
void service_clients(pool *p);

int main()
{
    int listenfd = socket(AF_INET, SOCK_STREAM, 0); // IPv4 + TCP
    if (listenfd < 0)
    {
        perror("socket()");
        exit(1);
    }

    // allow binding to the same `IP:port` after restart
    int val = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    // bind
    // (htons, htonl: convert values from host to network byte order)
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    int port = 8000;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(0); // 0.0.0.0 - listen on all network interfaces
    int rv = bind(listenfd, (const struct sockaddr *)&addr, sizeof(addr));
    if (rv < 0)
    {
        perror("bind()");
        exit(1);
    }

    // listen for incoming connections
    rv = listen(listenfd, SOMAXCONN);
    if (rv < 0)
    {
        perror("listen()");
        exit(1);
    }

    pool pool;
    init_pool(listenfd, &pool);

    printf("Server listening on port %d\n", port);

    // event loop
    while (1)
    {
        pool.ready_set = pool.read_set;
        pool.nready = select(pool.maxfd + 1, &pool.ready_set, NULL, NULL, NULL);
        if (pool.nready < 0)
        {
            perror("select()");
            break;
        }

        // if listenfd is ready, accept new client
        if (FD_ISSET(listenfd, &pool.ready_set))
        {
            struct sockaddr_in client_addr = {};
            socklen_t addrlen = sizeof(client_addr);
            int connfd = accept(listenfd, (struct sockaddr *)&client_addr, &addrlen);
            if (connfd < 0)
                perror("accept()");
            else
                add_client(connfd, client_addr, &pool);
        }

        // service ready client connections
        service_clients(&pool);
    }

    return 0;
}

/* Initialize pool of active clients. */
void init_pool(int listenfd, pool *p)
{
    p->maxi = -1;
    for (int i = 0; i < FD_SETSIZE; i++)
        p->clientfd[i] = -1;

    // Register listenfd
    p->maxfd = listenfd;
    FD_ZERO(&p->read_set);
    FD_SET(listenfd, &p->read_set);
}

/* Add new client to the pool. */
void add_client(int connfd, struct sockaddr_in client_addr, pool *p)
{
    int i;
    p->nready--; // event handled

    for (int i = 0; i < FD_SETSIZE; i++)
    {
        if (p->clientfd[i] < 0) // found an available slot
        {
            p->clientfd[i] = connfd;
            FD_SET(connfd, &p->read_set);

            if (connfd > p->maxfd)
                p->maxfd = connfd;
            if (i > p->maxi)
                p->maxi = i;

            printf("Connected client: %s:%d\n",
                   inet_ntoa(client_addr.sin_addr),
                   ntohs(client_addr.sin_port));

            break;
        }
    }

    if (i == FD_SETSIZE)
    {
        fprintf(stderr, "Too many clients. Rejecting client%s:%d\n",
                inet_ntoa(client_addr.sin_addr),
                ntohs(client_addr.sin_port));

        close(connfd);
    }
}

void remove_client(int i, pool *p)
{
    int connfd = p->clientfd[i];
    if (connfd > 0)
    {
        close(connfd);
        FD_CLR(connfd, &p->read_set);
        p->clientfd[i] = -1;
    }
}

void handle_request(int connfd)
{
    // Just return current time for now
    time_t rawtime;
    time(&rawtime);
    char *time_str = ctime(&rawtime);

    ssize_t n = write(connfd, time_str, strlen(time_str));
    if (n < 0)
        perror("write()");
}

/* Service ready client connections. */
void service_clients(pool *p)
{
    if (p->nready == 0)
        return;

    int i, connfd;
    ssize_t n;
    char buffer[1024];

    for (i = 0; i <= p->maxi; i++)
    {
        connfd = p->clientfd[i];

        if ((connfd > 0) && FD_ISSET(connfd, &p->ready_set))
        {
            p->nready--; // event handled
            n = read(connfd, buffer, sizeof(buffer));

            if (n < 0)
                perror("read()");
            else if (n == 0) // EOF
                printf("Client with fd %d disconnected\n", connfd);
            else
                handle_request(connfd);

            remove_client(i, p);
        }
    }
}