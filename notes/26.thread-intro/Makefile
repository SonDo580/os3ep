FLAGS = -Wall
INCLUDES = ../include
CC = gcc

.PHONY: all clean

all: t0 t1

clean: rm *.exe

t0: t0.c
	gcc -I $(INCLUDES) -o t0.exe t0.c $(FLAGS)

t1: t1.c
	gcc -I $(INCLUDES) -o t1.exe t1.c $(FLAGS)