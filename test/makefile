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

TESTS = t_vec$(_EXE) t_buf$(_EXE) t_mem$(_EXE) t_pmx$(_EXE) t_pmx2$(_EXE)  t_pmx3$(_EXE) t_logassert$(_EXE)
		 
.SUFFIXES: .c .h .o

DBGFLAGS =
WRNFLAGS = -Wall -pedantic
PRFFLAGS =
CCFLAGS  = -std=c99 

CFLAGS= $(PRFFLAGS) $(WRNFLAGS) $(CCFLAGS) $(DBGFLAGS) -I../src 
LNFLAGS = $(PRFFLAGS) -L../src/

LIBUTL = ../src/libutl.a

all:  $(TESTS)

prof:
	make PRFFLAGS=-pg

t_vec$(_EXE): $(LIBUTL) ut_vec.o
	$(CC) $(LNFLAGS) -o $@ ut_vec.o -lutl

t_buf$(_EXE): $(LIBUTL)  ut_buf.o
	$(CC) $(LNFLAGS) -o $@ ut_buf.o -lutl

t_pmx$(_EXE): $(LIBUTL)  ut_pmx.o
	$(CC) $(LNFLAGS) -o $@ ut_pmx.o -lutl
  
t_pmx2$(_EXE): $(LIBUTL)  ut_pmx2.o
	$(CC) $(LNFLAGS) -o $@ ut_pmx2.o -lutl

t_pmx3$(_EXE): $(LIBUTL)  ut_pmx3.o
	$(CC) $(LNFLAGS) -o $@ ut_pmx3.o -lutl

t_mem$(_EXE): $(LIBUTL)  ut_mem.o
	$(CC) $(LNFLAGS) -o $@ ut_mem.o -lutl

t_logassert$(_EXE): $(LIBUTL) ut_logassert.c
	$(CC) $(LNFLAGS) -o $@ ut_logassert.o -lutl

$(LIBUTL): $(UTL_H)
	cd ../src; make
  
clean:
	rm -f t_* *.o *.tmp *.log gmon.out

