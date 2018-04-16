CC=gcc
CFLAGS=-Wall

all: proj3

proj3: sisinger_andrew_proj3_COMP323.c proj3.h
	$(CC) $(CFLAGS) -o proj3 sisinger_andrew_proj3_COMP323.c

clean:
	rm proj3
