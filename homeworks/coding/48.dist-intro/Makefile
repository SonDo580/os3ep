CC = gcc
cflags = -g -Wall -Wextra
CFLAGS = $(cflags)

.PHONY: all clean

all: client.exe server.exe

client.exe: client.c common.h
	$(CC) $(CFLAGS) client.c -o client.exe

server.exe: server.c common.h
	$(CC) $(CFLAGS) server.c -o server.exe

clean:
	rm -f *.exe
