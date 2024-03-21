CC=gcc
CFLAGS= -c -std=c11 -D_DEFAULT_SOURCE -Wall

all: a.out 

a.out: main.o 
	$(CC) main.o -o a.out

main.o: main.c
	$(CC) $(CFLAGS) main.c

clean:
	rm -rf *.o a.out *.log *.txt
