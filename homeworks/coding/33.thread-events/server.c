#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>
#include <time.h>

volatile sig_atomic_t server_running = 1;

void sigint_handler(int sig)
{
    server_running = 0;
    printf("\nShutting down...\n");
}

// Pool of connected descriptors
typedef struct
{
    int maxfd;              // largest descriptor in read_set/write_set
    fd_set read_set;        // set of descriptors watched for reading (including listenfd)
    fd_set write_set;       // set of descriptors watched for writing (including listenfd)
    fd_set ready_read_set;  // subset of descriptors ready for reading
    fd_set ready_write_set; // subset of descriptors ready for writing
    int nready;             // number of ready descriptors from select()
    int maxi;               // high watermark index into clientfd array
    int clientfd[FD_SETSIZE];

    // TODO: This solution is too specific. What if there are other types of requests?
    FILE *client_file[FD_SETSIZE]; // (single) open file per client
} pool;

void init_pool(int listenfd, pool *p);
void add_client(int connfd, struct sockaddr_in client_addr, pool *p);
void service_clients(pool *p);
int has_active_clients(pool *p);

int main()
{
    signal(SIGINT, sigint_handler);

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
    while (server_running || has_active_clients(&pool))
    {
        // if shutting down, ensure listenfd is no longer watched
        if (!server_running && listenfd != -1)
        {
            close(listenfd);
            FD_CLR(listenfd, &pool.read_set);
            listenfd = -1; // closed
        }

        pool.ready_read_set = pool.read_set;
        pool.ready_write_set = pool.write_set;
        pool.nready = select(
            pool.maxfd + 1,
            &pool.ready_read_set,
            &pool.ready_write_set,
            NULL,
            NULL);

        if (pool.nready < 0)
        {
            if (errno == EINTR)
                continue;

            perror("select()");
            break;
        }

        // if not shutting down and listenfd is ready, accept new client
        if (server_running && FD_ISSET(listenfd, &pool.ready_read_set))
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
    {
        p->clientfd[i] = -1;
        p->client_file[i] = NULL;
    }

    FD_ZERO(&p->read_set);
    FD_ZERO(&p->write_set);

    // Register listenfd for read monitoring
    p->maxfd = listenfd;
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
        if (p->client_file[i] != NULL)
        {
            fclose(p->client_file[i]);
            p->client_file[i] = NULL;
        }

        close(connfd);

        FD_CLR(connfd, &p->read_set);
        FD_CLR(connfd, &p->write_set);

        p->clientfd[i] = -1;
    }
}

void handle_request(int connfd, char *filename, pool *p, int i)
{
    printf("Requested file: %s\n", filename);

    FILE *file = fopen(filename, "r");
    if (!file)
    {
        char *error_msg = "Error: File not found or cannot be opened\n";
        write(connfd, error_msg, strlen(error_msg));
        remove_client(i, p);
        return;
    }

    // Save state: store file pointer and register connfd for write monitoring
    p->client_file[i] = file;
    FD_SET(connfd, &p->write_set);
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
        if (connfd < 0)
            continue;

        // Handle read readiness (incoming request)
        if (FD_ISSET(connfd, &p->ready_read_set))
        {
            p->nready--; // event handled
            n = read(connfd, buffer, sizeof(buffer) - 1);

            if (n < 0)
            {
                perror("read()");
                remove_client(i, p);
            }
            else if (n == 0) // EOF
            {
                printf("Client with fd %d disconnected\n", connfd);
                remove_client(i, p);
            }
            else
            {
                buffer[n] = '\0'; // filename
                handle_request(connfd, buffer, p, i);
            }
        }

        // Handle write readiness
        if (FD_ISSET(connfd, &p->ready_write_set))
        {
            p->nready--; // event handled
            FILE *file = p->client_file[i];
            if (!file)
                continue;

            // Read 1 chunk from the file and stream it to client
            size_t nread = fread(buffer, 1, sizeof(buffer), file);
            if (nread > 0)
            {
                ssize_t nwritten = write(connfd, buffer, nread);
                if (nwritten < 0 && errno != EWOULDBLOCK && errno != EAGAIN)
                { // if socket buffer is full, try again
                    perror("write()");
                    remove_client(i, p);
                    continue;
                }
            }

            if (nread < sizeof(buffer)) // error or EOF
            {
                if (ferror(file))
                    perror("fread()");
                remove_client(i, p);
            }
        }
    }
}

int has_active_clients(pool *p)
{
    for (int i = 0; i <= p->maxi; i++)
    {
        if (p->clientfd[i] != -1)
            return 1;
    }
    return 0;
}