CC = gcc
CFLAGS = -g -Wall -Wextra

.PHONY: all clean 

all: server.exe client.exe

server.exe: server.c
	$(CC) $(FLAGS) server.c -o server.exe

client.exe: client.c
	$(CC) $(FLAGS) client.c -o client.exe

clean: 
	rm *exe