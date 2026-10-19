#include <stdio.h>

#include "request.h"
#include "io_helper.h"
#include "conn_queue.h"
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
	char *sched_algo = "FIFO";

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
			if (strcmp(optarg, "FIFO") && strcmp(optarg, "SFF"))
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

	// init queue of connfd's
	ConnQueue q;
	conn_queue_init(&q, buffer_size);

	// init pool of worker threads
	ThreadPool tp;
	thread_pool_init(&tp, num_threads, &q);

	while (1)
	{
		struct sockaddr_in client_addr;
		int client_len = sizeof(client_addr);
		int conn_fd = accept_or_die(listen_fd, (sockaddr_t *)&client_addr, (socklen_t *)&client_len);
		conn_queue_produce(&q, conn_fd);
	}
	return 0;
}
