CC=gcc
CFLAGS=-std=c11 -g -Wall -Wextra

SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

asari-lox: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f asari-lox *.o 

.PHONY:
	clean