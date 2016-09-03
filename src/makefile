# 
#  (C) 2009 Remo Dentato (rdentato@gmail.com)
# 
# This software is distributed under the terms of the MIT license:
#   https://opensource.org/licenses/MIT
#

_EXE=.exe
ifeq "$(COMSPEC)" ""
_EXE=
endif

CC=gcc
AR=ar -ru
RM=rm -f

all:  libutl.a

libutl.a: utl.c utl.h
	$(CC) -O2 -c utl.c
	$(AR) $@ utl.o

clean:
	$(RM) libutl.a *.o
