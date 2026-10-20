#include <stdio.h>

#include "common.h"
#include "request.h"
#include "io_helper.h"
#include "conn_queue.h"
#include "conn_heap.h"
#include "thread_pool.h"

char default_root[] = ".";
char *usage_msg = "usage: wserver [-d basedir] [-p port] [-t <threads>] [-b <buffers>] [-s <schedalg (FIFO|SFF)>]\n";

int main(int argc, char *argv[])
{
	int c;
	char *root_dir = default_root;
	int port = 10000;
	int num_threads = 1;
	int buffer_size = 1;
	char *sched_algo = SchedAlgoFIFO;

	while ((c = getopt(argc, argv, "d:p:t:b:s:")) != -1)
		switch (c)
		{
		case 'd':
			root_dir = optarg;
			break;
		case 'p':
			port = atoi(optarg);
			break;
		case 't':
			num_threads = atoi(optarg);
			break;
		case 'b':
			buffer_size = atoi(optarg);
			break;
		case 's':
			if (strcmp(optarg, SchedAlgoFIFO) && strcmp(optarg, SchedAlgoSFF))
			{
				fprintf(stderr, usage_msg);
				exit(1);
			}
			sched_algo = optarg;
			break;
		default:
			fprintf(stderr, usage_msg);
			exit(1);
		}

	// run out of this directory
	chdir_or_die(root_dir);

	// now, get to work
	int listen_fd = open_listen_fd_or_die(port);

	// init queue/heap of connfd's
	ConnQueue *q = NULL;
	ConnHeap *h = NULL;
	if (strcmp(sched_algo, SchedAlgoFIFO) == 0)
	{
		q = Malloc(sizeof(ConnQueue));
		conn_queue_init(q, buffer_size);
	}
	else if (strcmp(sched_algo, SchedAlgoSFF) == 0)
	{
		h = Malloc(sizeof(ConnHeap));
		conn_heap_init(h, buffer_size);
	}

	// init pool of worker threads
	ThreadPool tp;
	thread_pool_init(&tp, num_threads, sched_algo, q, h);

	while (1)
	{
		struct sockaddr_in client_addr;
		int client_len = sizeof(client_addr);
		int conn_fd = accept_or_die(listen_fd, (sockaddr_t *)&client_addr, (socklen_t *)&client_len);

		if (strcmp(sched_algo, SchedAlgoFIFO) == 0)
			conn_queue_produce(q, conn_fd);
		else if (strcmp(sched_algo, SchedAlgoSFF) == 0)
		{
			RequestInfo req;
			if (request_handle_http_line(conn_fd, &req) == -1)
			{
				close_or_die(conn_fd);
				continue;
			}

			ConnHeapItem item = {
				.filesize = req.sbuf.st_size,
				.connfd = conn_fd,
				.req = &req,
			};
			conn_heap_produce(h, item);
		}
	}

	if (q != NULL)
	{
		conn_queue_free(q);
		free(q);
	}
	if (h != NULL)
	{
		conn_heap_free(h);
		free(h);
	}
	thread_pool_free(&tp);

	return 0;
}
