CC = gcc
FLAGS = -Wall -g

.PHONY: clean

all: fork-join.exe rendezvous.exe

fork-join.exe: fork-join.c common_threads.h
	$(CC) -o $@ $< $(FLAGS)

rendezvous.exe: rendezvous.c common_threads.h
	$(CC) -o $@ $< $(FLAGS)

clean:
	rm *.exe