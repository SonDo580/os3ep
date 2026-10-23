CC = gcc
CFLAGS = -g -Wall -Wextra

.PHONY: all clean

all: stat.exe ls.exe tail.exe search.exe
 
stat.exe: stat.c
	$(CC) $(CFLAGS) -o $@ $<

ls.exe: ls.c
	$(CC) $(CFLAGS) -o $@ $<

tail.exe: tail.c
	$(CC) $(CFLAGS) -o $@ $<

search.exe: search.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm *.exe