CC=gcc
CFLAGS=-std=c11 -g -Wall -Wextra -DDEBUG

SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

asari-lox: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

run: asari-lox
	./run.sh

clean:
	rm -f asari-lox *.o 

.PHONY:
	run clean