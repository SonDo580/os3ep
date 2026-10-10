FLAGS = -Wall -g
CC = gcc

.PHONY: clean

clean:
	rm *.exe

1.test.exe: 1.test.c
	$(CC) -o $@ $< $(FLAGS)

2.concurrent-counter.exe: 2.concurrent-counter.c common_threads.h
	$(CC) -o $@ $< $(FLAGS)

3.approximate-counter.exe: 3.approximate-counter.c common_threads.h
	$(CC) -o $@ $< $(FLAGS)