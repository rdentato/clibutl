# 
#  (C) by Remo Dentato (rdentato@gmail.com)
# 
# This software is distributed under the terms of the BSD license:
#   http://creativecommons.org/licenses/BSD/
#   http://opensource.org/licenses/bsd-license.php
#

_EXE=.exe
_OBJ=o
_LIB=a

CC=gcc
AR=ar -ru

.SUFFIXES: .c .h $(_OBJ)

CFLAGS=-O3

.c.o:
	$(CC) $(CFLAGS) -c -o $*.$(_OBJ) $*.c

LIBOBJ = utl.$(_OBJ)

libutl.$(_LIB) : $(LIBOBJ) 
	$(AR) $@ $(LIBOBJ)

clean:
	rm -f *.exe *.$(_OBJ) *.$(_LIB) *.tmp

