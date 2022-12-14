CFLAGS= -g -Wall -Wextra -pedantic

all: cshell

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $^

cshell: main.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -r *.o *.dSYM cshell
