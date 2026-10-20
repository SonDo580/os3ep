#ifndef __REQUEST_H__
#define __REQUEST_H__

#include <sys/stat.h>

#define MAXBUF (8192)

typedef struct __RequestInfo
{
    char method[MAXBUF];
    char uri[MAXBUF];
    char version[MAXBUF];
    char filename[MAXBUF];
    char cgiargs[MAXBUF];
    struct stat sbuf;
    int is_static;
} RequestInfo;

int request_handle_http_line(int fd, RequestInfo *req);
void request_handle_after_http_line(int fd, RequestInfo *req);
void request_handle(int fd, RequestInfo *req);

#endif // __REQUEST_H__
