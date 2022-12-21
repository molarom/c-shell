SOURCES := $(wildcard *.c)
OBJECTS := $(patsubst %.c,%.o,$(SOURCES))
DEPENDS := $(patsubst %.c,%.d,$(SOURCES))

CFLAGS= -g -Wall -Wextra -pedantic

.PHONY: all clean

all: cshell

cshell: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

-include $(DEPENDS)

%.o: %.c 
	$(CC) $(CFLAGS) -c $^

clean:
	rm -r *.o *.dSYM cshell
