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
#CC=gcc
_BAT=.sh
CP=cp

UTL_H=../src/utl.h

TESTS = t_vec$(_EXE) t_buf$(_EXE)  t_mem$(_EXE)  t_mem2$(_EXE)\
        t_pmx$(_EXE) t_buf2$(_EXE) t_pmx2$(_EXE) t_pmx3$(_EXE) \
        t_logassert$(_EXE)
		 
.SUFFIXES: .c .h .o

DBGFLAGS =
WRNFLAGS = -Wall -pedantic
PRFFLAGS =
CCFLAGS  = -std=c99 

CFLAGS= $(PRFFLAGS) $(WRNFLAGS) $(CCFLAGS) $(DBGFLAGS) -I../src 
LNFLAGS = $(PRFFLAGS) -L../src/

LIBUTL = ../src/libutl.a
HDR_SINGLE = ../src/utl_single.h

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

t_logassert$(_EXE): $(LIBUTL) ut_logassert.o
	$(CC) $(LNFLAGS) -o $@ ut_logassert.o -lutl

# Test using `utl_single.h`
t_mem2$(_EXE): $(HDR_SINGLE)  ut_mem2.o
	$(CC) $(LNFLAGS) -o $@ ut_mem2.o

t_buf2$(_EXE): $(HDR_SINGLE)  ut_buf2.o
	$(CC) $(LNFLAGS) -o $@ ut_buf2.o

  
## -----------

$(LIBUTL): 
	cd ../src; make libutl.a

$(HDR_SINGLE): 
	cd ../src; make utl_single.h

  
coverage:
	cd ../src; make coverage
	$(CP) ../src/utl.c .
	make "PRFFLAGS=-fprofile-arcs -ftest-coverage"
 
profile:
	cd ../src; make profile
	$(CP) ../src/utl.c .
	make "PRFFLAGS=-pg"
  
clean:
	rm -f t_* *.o *.tmp *.log gmon.out *.gc?? utl.c

