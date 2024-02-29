CC = gcc
CFLAGS = -g -Wall -Wextra -Werror -std=c99

SRCS = string_reverse.c
OBJS = $(SRCS:.c=.o)

all: string_reverse string_reverse_child

string_reverse: string_reverse.o
	$(CC) $(CFLAGS) -o $@ $^

string_reverse_child: string_reverse_child.o
	$(CC) $(CFLAGS) -o $@ $^

string_reverse.o: string_reverse.c
	$(CC) $(CFLAGS) -c $< -o $@

string_reverse_child.o: string_reverse_child.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f string_reverse string_reverse_child *.o