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
#include <fcntl.h>
#include <assert.h>

volatile sig_atomic_t server_running = 1;
int sig_pipe[2];

void shutdown_signal_handler(int sig)
{
    server_running = 0;

    // Write a single byte to the pipe to wake up event loop thread
    char val = 1;
    write(sig_pipe[1], &val, sizeof(char));
}

void setup_signals()
{
    struct sigaction sa = {};
    sa.sa_handler = shutdown_signal_handler;
    sigemptyset(&sa.sa_mask); // no signals are blocked while the handler is running
    sa.sa_flags = 0;          // standard behavior

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
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

void make_nonblocking(int fd);
void init_pool(int listenfd, pool *p);
void add_client(int connfd, struct sockaddr_in client_addr, pool *p);
void service_clients(pool *p);
int has_active_clients(pool *p);

int main()
{
    // Setup self-pipe and signals
    if (pipe(sig_pipe) < 0)
    {
        perror("pipe()");
        exit(1);
    }
    make_nonblocking(sig_pipe[0]);
    make_nonblocking(sig_pipe[1]);
    setup_signals();

    int listenfd = socket(AF_INET, SOCK_STREAM, 0); // IPv4 + TCP
    if (listenfd < 0)
    {
        perror("socket()");
        exit(1);
    }

    // make listening socket nonblocking
    make_nonblocking(listenfd);

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
    if (bind(listenfd, (const struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind()");
        exit(1);
    }

    // listen for incoming connections
    if (listen(listenfd, SOMAXCONN) < 0)
    {
        perror("listen()");
        exit(1);
    }

    pool pool;
    init_pool(listenfd, &pool);

    // register read end of signal pipe for read monitoring
    FD_SET(sig_pipe[0], &pool.read_set);
    if (sig_pipe[0] > pool.maxfd)
        pool.maxfd = sig_pipe[0];

    printf("Server listening on port %d\n", port);

    // event loop
    while (server_running || has_active_clients(&pool))
    {
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
            else
            {
                perror("select()");
                break;
            }
        }

        // check if signal pipe has data (SIGINT or SIGTERM was caught)
        if (FD_ISSET(sig_pipe[0], &pool.ready_read_set))
        {
            // drain pipe (so select() doesn't trigger again on next iteration)
            char buf[16];
            while (read(sig_pipe[0], buf, sizeof(buf)) > 0)
                ;

            const char msg[] = "\nShutting down...\n";
            write(STDOUT_FILENO, msg, sizeof(msg) - 1); // why -1: don't write '\0'

            // ensure listenfd is no longer watched
            assert(listenfd != -1);
            close(listenfd);
            FD_CLR(listenfd, &pool.read_set);
            listenfd = -1; // closed
        }

        // if not shutting down and listenfd is ready, accept new client
        if (server_running && FD_ISSET(listenfd, &pool.ready_read_set))
        {
            struct sockaddr_in client_addr = {};
            socklen_t addrlen = sizeof(client_addr);
            int connfd = accept(listenfd, (struct sockaddr *)&client_addr, &addrlen);
            if (connfd < 0)
            {
                if (errno != EAGAIN && errno != EWOULDBLOCK)
                    perror("accept()");
            }
            else
            {
                make_nonblocking(connfd); // Make connected socket nonblocking
                add_client(connfd, client_addr, &pool);
            }
        }

        // service ready client connections
        service_clients(&pool);
    }

    // Clean up
    close(sig_pipe[0]);
    close(sig_pipe[1]);

    return 0;
}

/* Set a file descriptor to non-blocking mode */
void make_nonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
    {
        perror("fcntl()");
        exit(1);
    }

    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        perror("fcntl()");
        exit(1);
    }
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
                if (errno != EAGAIN && errno != EWOULDBLOCK)
                {
                    perror("read()");
                    remove_client(i, p);
                }
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