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

_OBJ=o
CC=gcc
_BAT=.sh

UTL_H=../src/utl.h

TESTS = t_vec$(_EXE) t_buf$(_EXE) t_mem$(_EXE) t_pmx$(_EXE)
		 
.SUFFIXES: .c .h .o

DBGFLAGS =
WRNFLAGS = -Wall
PRFFLAGS =
CCFLAGS  = 

CFLAGS= $(PRFFLAGS) $(WRNFLAGS) $(CCFLAGS) $(DBGFLAGS) -I../src 

all:  $(TESTS)

prof:
	make PRFFLAGS=-pg

t_vec$(_EXE): $(UTL_H) ut_vec.c
	$(CC) $(CFLAGS) -c ut_vec.c
	$(CC) $(PRFFLAGS) -o $@ ut_vec.o

t_buf$(_EXE): $(UTL_H)  ut_buf.c
	$(CC) $(CFLAGS) -c ut_buf.c
	$(CC) $(PRFFLAGS) -o $@ ut_buf.o

t_pmx$(_EXE): $(UTL_H)  ut_pmx.c
	$(CC) $(CFLAGS) -c ut_pmx.c
	$(CC) $(PRFFLAGS) -o $@ ut_pmx.o

t_mem$(_EXE): $(UTL_H)  ut_mem.c
	$(CC) $(CFLAGS) -c ut_mem.c
	$(CC) $(PRFFLAGS) -o $@ ut_mem.o

clean:
	rm -f t_* *.o *.tmp *.log gmon.out

