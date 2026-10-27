CC = gcc
CFLAGS = -g -Wall -Wextra

.PHONY: all clean csum

all: check-xor.exe check-fletcher.exe csum
 
check-xor.exe: check-xor.c
	$(CC) $(CFLAGS) -o $@ $<

check-fletcher.exe: check-fletcher.c
	$(CC) $(CFLAGS) -o $@ $<

csum: create-csum.exe check-csum.exe

create-csum.exe: create-csum.c
	$(CC) $(CFLAGS) -o $@ $<

check-csum.exe: check-csum.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm *.exe