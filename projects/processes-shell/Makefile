CC = gcc
FLAGS = -g -Wall -Werror

all: wish test-parse

wish: wish.c common.h common.c command.h command.c path.h path.c parse.h parse.c exec.h exec.c
	$(CC) $(FLAGS) -o $@ wish.c common.c command.c path.c parse.c exec.c

test-parse: test-parse.c common.h common.c command.h command.c parse.h parse.c
	$(CC) $(FLAGS) -o $@ test-parse.c common.c command.c parse.c

.PHONY: clean
clean:
	rm wish test-parse