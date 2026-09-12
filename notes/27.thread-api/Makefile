CC := gcc
CFLAGS := -Wall -Werror -I../include 

SRCS := $(wildcard *.c)
PROGS  := $(SRCS:.c=)

.PHONY: all clean

all: $(PROGS)

clean:
	rm -f *.exe

$(PROGS) : % : %.c
	$(CC) $(CFLAGS) $< -o $@.exe
