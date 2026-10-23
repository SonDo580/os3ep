CC = gcc
CFLAGS = -g -Wall -Wextra

.PHONY: all clean

all: stat.exe ls.exe
 
stat.exe: stat.c
	$(CC) $(CFLAGS) -o $@ $<

ls.exe: ls.c
	$(CC) $(CFLAGS) -o $@ $<

tail.exe: tail.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm *.exe