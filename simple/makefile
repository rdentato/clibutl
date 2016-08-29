# 
#  (C) 2009 Remo Dentato (rdentato@gmail.com)
# 
# This software is distributed under the terms of the BSD license:
#   http://opensource.org/licenses/bsd-license.php
#


_EXE=.exe

ifeq "$(COMSPEC)" ""
_EXE=
endif

_OBJ=o
CC=gcc
_BAT=.sh

UTL_H=utl.h

TESTS = t_vec$(_EXE) t_mem$(_EXE)

		 
.SUFFIXES: .c .h $(_OBJ)


DBGFLAGS =
WRNFLAGS = -Wall
PRFFLAGS =

CFLAGS= $(PRFFLAGS) $(WRNFLAGS) $(CCFLAGS) $(DBGFLAGS) -I../src 

.c.o:
	$(CC) $(CFLAGS) -c -o $*.$(_OBJ) $*.c

all:  $(TESTS)

prof:
	make PRFFLAGS=-pg

t_vec$(_EXE): $(UTL_H)  ut_vec.c
	$(CC) $(CFLAGS) -c -o ut_vec.$(_OBJ) ut_vec.c
	$(CC) $(PRFFLAGS) -o $@ ut_vec.$(_OBJ)

t_mem$(_EXE): $(UTL_H)  ut_mem.c
	$(CC) $(CFLAGS) -c -o ut_mem.$(_OBJ) ut_mem.c
	$(CC) $(PRFFLAGS) -o $@ ut_mem.$(_OBJ)

clean:
	rm -f t_*$(_EXE) *.$(_OBJ) *.tmp *.log gmon.out

