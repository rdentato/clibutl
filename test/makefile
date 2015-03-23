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

TESTS = t_buf$(_EXE)      t_que$(_EXE)      t_vec$(_EXE)   \
        t_log$(_EXE)      t_general$(_EXE)  t_try$(_EXE)  \
        t_try2$(_EXE)     t_mem$(_EXE)      t_fsm$(_EXE)  \
	      t_vecperf$(_EXE)  t_match$(_EXE)\
		 
.SUFFIXES: .c .h $(_OBJ)


DBGFLAGS =
WRNFLAGS = -Wall
PRFFLAGS =

CFLAGS= $(PRFFLAGS) $(WRNFLAGS) $(CCFLAGS) $(DBGFLAGS) -I../src 

.c.o:
	$(CC) $(CFLAGS) -c -o $*.$(_OBJ) $*.c

all:  $(TESTS)
	chmod +x ./run_tests.sh
	./run_tests.sh > tests.log
	sed -n -e '/TST RES/p' < tests.log

prof:
	make PRFFLAGS=-pg

t_que$(_EXE): $(UTL_H)  utl_que_ut.c
	$(CC) $(CFLAGS) -c -o utl_que_ut.$(_OBJ) utl_que_ut.c
	$(CC) $(PRFFLAGS) -o $@ utl_que_ut.$(_OBJ)

t_buf$(_EXE): $(UTL_H)  utl_buf_ut.c
	$(CC) $(CFLAGS) -c -o utl_buf_ut.$(_OBJ) utl_buf_ut.c
	$(CC) $(PRFFLAGS) -o $@ utl_buf_ut.$(_OBJ)

t_vec$(_EXE): $(UTL_H)  utl_vec_ut.c
	$(CC) $(CFLAGS) -c -o utl_vec_ut.$(_OBJ) utl_vec_ut.c
	$(CC) $(PRFFLAGS) -o $@ utl_vec_ut.$(_OBJ)

t_vecperf$(_EXE): $(UTL_H)  utl_vecperf_ut.c
	$(CC) $(CFLAGS) -c -o utl_vecperf_ut.$(_OBJ) utl_vecperf_ut.c
	$(CC) $(PRFFLAGS) -o $@ utl_vecperf_ut.$(_OBJ)

GLIB_INC=$(shell pkg-config --cflags glib-2.0)
GLIB_LIB=$(shell pkg-config --libs glib-2.0)

t_gvecperf$(_EXE): $(UTL_H)  utl_gvecperf_ut.c
	$(CC) $(CFLAGS) $(GLIB_INC) -c -o utl_gvecperf_ut.$(_OBJ) utl_gvecperf_ut.c
	$(CC) $(PRFFLAGS) -o $@ utl_gvecperf_ut.$(_OBJ) $(GLIB_LIB)

t_log$(_EXE): $(UTL_H)  utl_logging_ut.c
	$(CC) $(CFLAGS) -c -o utl_logging_ut.$(_OBJ) utl_logging_ut.c
	$(CC) $(PRFFLAGS) -o $@ utl_logging_ut.$(_OBJ)

t_general$(_EXE): $(UTL_H)  utl_general_ut.c
	$(CC) $(CFLAGS) -c -o utl_general_ut.$(_OBJ) utl_general_ut.c
	$(CC) $(PRFFLAGS) -o $@ utl_general_ut.$(_OBJ)

t_try$(_EXE): $(UTL_H)  utl_exception_ut.c
	$(CC) $(CFLAGS) -c -o utl_exception_ut.$(_OBJ) utl_exception_ut.c
	$(CC) $(PRFFLAGS) -o $@ utl_exception_ut.$(_OBJ)

t_try2$(_EXE): $(UTL_H)  utl_exception2_ut.c
	$(CC) $(CFLAGS) -c -o utl_exception2_ut.$(_OBJ) utl_exception2_ut.c
	$(CC) $(PRFFLAGS) -o $@ utl_exception2_ut.$(_OBJ)

t_mem$(_EXE): $(UTL_H)  utl_memory_ut.c
	$(CC) $(CFLAGS) -c -o utl_memory_ut.$(_OBJ) utl_memory_ut.c
	$(CC) $(PRFFLAGS) -o $@ utl_memory_ut.$(_OBJ)

t_fsm$(_EXE): $(UTL_H)  utl_fsm_ut.c
	$(CC) $(CFLAGS) -c -o utl_fsm_ut.$(_OBJ) utl_fsm_ut.c
	$(CC) $(PRFFLAGS) -o $@ utl_fsm_ut.$(_OBJ)

t_match$(_EXE): $(UTL_H)  utl_match_ut.c
	$(CC) $(CFLAGS) -c -o utl_match_ut.$(_OBJ) utl_match_ut.c
	$(CC) $(PRFFLAGS) -o $@ utl_match_ut.$(_OBJ)

clean:
	rm -f t_*$(_EXE) *.$(_OBJ) *.tmp *.log gmon.out

