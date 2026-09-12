FLAGS = -Wall -g

.PHONY: all clean

all: main-race main-race-fix main-deadlock main-deadlock-global main-signal main-signal-cv

clean:
	rm *.exe

main-race: main-race.c common_threads.h
	gcc -o main-race.exe main-race.c $(FLAGS)

main-race-fix: main-race-fix.c common_threads.h
	gcc -o main-race-fix.exe main-race-fix.c $(FLAGS)

main-deadlock: main-deadlock.c common_threads.h
	gcc -o main-deadlock.exe main-deadlock.c $(FLAGS)

main-deadlock-global: main-deadlock-global.c common_threads.h
	gcc -o main-deadlock-global.exe main-deadlock-global.c $(FLAGS)

main-signal: main-signal.c common_threads.h
	gcc -o main-signal.exe main-signal.c $(FLAGS)

main-signal-cv: main-signal-cv.c common_threads.h
	gcc -o main-signal-cv.exe main-signal-cv.c $(FLAGS)