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

UTL_H=../src/utl.h


.SUFFIXES: .c .h $(_OBJ)

CFLAGS= $(CCFLAGS) -I../../src -g -Wall

.c.$(_OBJ):
	$(CC) $(CFLAGS) -c -o $*.$(_OBJ) $*.c

all:  match$(_EXE)
	
match$(_EXE): match.c
	$(CC) $(CFLAGS) -c -o match.$(_OBJ) match.c
	$(CC) -o $@ match.$(_OBJ)

clean:
	rm -f match$(_EXE) *.$(_OBJ) *.tmp *.log

