CC = gcc
CFLAGS = -Wall -lrt

filecopy: filecopy.c
	$(CC) $(CFLAGS) -o filecopy filecopy.c
	./filecopy filecopy.c filecopy.c.copy

clean:
	rm -f filecopy
