CC = gcc
CFLAGS = -g -Wall -Wextra

.PHONY: all clean

all: stat.exe
 
stat.exe: stat.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm *.exe