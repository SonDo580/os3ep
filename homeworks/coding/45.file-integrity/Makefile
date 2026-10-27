CC = gcc
CFLAGS = -g -Wall -Wextra

.PHONY: all clean

all: check-xor.exe
 
check-xor.exe: check-xor.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm *.exe