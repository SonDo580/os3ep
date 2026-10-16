CC = gcc
FLAGS = -Wall -g

.PHONY: clean

all: fork-join.exe rendezvous.exe barrier.exe reader-writer.exe

fork-join.exe: fork-join.c common_threads.h
	$(CC) -o $@ $< $(FLAGS)

rendezvous.exe: rendezvous.c common_threads.h
	$(CC) -o $@ $< $(FLAGS)

barrier.exe: barrier.c common_threads.h
	$(CC) -o $@ $< $(FLAGS)

reader-writer.exe: reader-writer.c common_threads.h
	$(CC) -o $@ $< $(FLAGS)

clean:
	rm *.exe