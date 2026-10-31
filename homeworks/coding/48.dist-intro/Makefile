CC = gcc
cflags = -g -Wall -Wextra
CFLAGS = $(cflags)

.PHONY: all clean

all: client.exe server.exe

client.exe: client.c comm_lib.h comm_lib.c
	$(CC) $(CFLAGS) -o client.exe client.c comm_lib.c

server.exe: server.c comm_lib.h comm_lib.c
	$(CC) $(CFLAGS) -o server.exe server.c comm_lib.c

clean:
	rm -f *.exe
