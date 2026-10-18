#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <time.h>

void handle_request(int connfd)
{
    // Just return current time for now
    time_t rawtime;
    time(&rawtime);
    char *time_str = ctime(&rawtime);

    ssize_t nbytes = write(connfd, time_str, strlen(time_str));
    if (nbytes < 0)
    {
        perror("write()");
        return;
    }
}

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

    printf("Server listening on port %d\n", port);

    // main loop: serve 1 request at a time
    while (1)
    {
        // accept connection
        struct sockaddr_in client_addr = {};
        socklen_t addrlen = sizeof(client_addr);
        int connfd = accept(listenfd, (struct sockaddr *)&client_addr, &addrlen);
        if (connfd < 0)
        {
            perror("accept()");
            continue; // don't crash the server
        }

        printf("Connected client: %s:%d\n",
               inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port));

        handle_request(connfd);

        close(connfd);
    }

    return 0;
}