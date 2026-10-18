#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ip.h>

void send_request(int clientfd, const char *request)
{
    printf("Sending request: %s\n", request);

    ssize_t n = write(clientfd, request, strlen(request));
    if (n < 0)
    {
        perror("write()");
        exit(1);
    }
}

void handle_response(int clientfd)
{
    char buffer[1024];
    ssize_t n;

    // (read() will write from buffer[0] each time)
    while ((n = read(clientfd, buffer, sizeof(buffer) - 1)) > 0)
    {
        buffer[n] = '\0'; // null-terminate the chunk for printing
        printf("%s", buffer);
    }

    // n == 0 means EOF (server closed connection)
    if (n < 0)
        perror("read()");
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "usage: ./client.exe <filename>");
        exit(1);
    }
    char *filename = argv[1];

    int clientfd = socket(AF_INET, SOCK_STREAM, 0); // IPv4 + TCP
    if (clientfd < 0)
    {
        perror("socket()");
        exit(1);
    }

    // connect to server
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8000);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // 127.0.0.1 (localhost)
    int rv = connect(clientfd, (const struct sockaddr *)&addr, sizeof(addr));
    if (rv < 0)
    {
        perror("connect()");
        exit(1);
    }

    // Random delay before sending request
    srand(getpid());
    int delay = rand() % 3 + 1; // 1 to 3 seconds
    sleep(delay);

    send_request(clientfd, filename);

    handle_response(clientfd);

    close(clientfd);

    return 0;
}