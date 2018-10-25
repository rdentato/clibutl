
#  Simplified makefile
#  Unix make file for random test program

CC = gcc
CFLAGS = -g -Wall
#CFLAGS = -O

ent:	ent.o iso8859.o randtest.o chisq.o
	$(CC) $(CFLAGS) ent.o iso8859.o randtest.o chisq.o -o ent -lm

ent.c:	iso8859.h randtest.h

clean:
	rm -f *.o *.bak ent ent.zip
