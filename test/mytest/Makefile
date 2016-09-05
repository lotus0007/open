.SUFFIXES:.c .o
CC=gcc
CFLAGES=-Wall
WORKDIR=.
src=$(wildcard *.c)

ALL:a.out
INCLUDEDIR= -I$(WORKDIR)/inc/
a.out:$(src)
	$(CC) $^  $(CFLAGES) $(INCLUDEDIR) -o a.out

.PHONY:clean
clean:
	rm -rf a.out

