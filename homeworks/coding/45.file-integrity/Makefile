CC = gcc
CFLAGS = -g -Wall -Wextra

.PHONY: all clean

all: check-xor.exe check-fletcher.exe
 
check-xor.exe: check-xor.c
	$(CC) $(CFLAGS) -o $@ $<

check-fletcher.exe: check-fletcher.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm *.exe