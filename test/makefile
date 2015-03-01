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

TESTS = t_buf$(_EXE)     t_vec$(_EXE)  t_log$(_EXE)   \
        t_general$(_EXE) t_try$(_EXE)  t_try2$(_EXE)  \
		t_mem$(_EXE)     t_fsm$(_EXE)  t_que$(_EXE)  

TESTS = t_buf$(_EXE)     t_que$(_EXE)      t_vec$(_EXE)   \
        t_log$(_EXE)     t_general$(_EXE)  t_try$(_EXE)  \
        t_try2$(_EXE)    t_mem$(_EXE)      t_fsm$(_EXE)  \
		
.SUFFIXES: .c .h $(_OBJ)

CFLAGS= $(CCFLAGS) -I../src -g -Wall

.c.o:
	$(CC) $(CFLAGS) -c -o $*.$(_OBJ) $*.c

all:  $(TESTS)
	chmod +x ./run_tests.sh
	./run_tests.sh > tests.log
	sed -n -e '/TST RES/p' < tests.log

t_que$(_EXE): $(UTL_H)  utl_que_ut.c
	$(CC) $(CFLAGS) -c -o utl_que_ut.$(_OBJ) utl_que_ut.c
	$(CC) -o $@ utl_que_ut.$(_OBJ)

t_buf$(_EXE): $(UTL_H)  utl_buf_ut.c
	$(CC) $(CFLAGS) -c -o utl_buf_ut.$(_OBJ) utl_buf_ut.c
	$(CC) -o $@ utl_buf_ut.$(_OBJ)

t_vec$(_EXE): $(UTL_H)  utl_vec_ut.c
	$(CC) $(CFLAGS) -c -o utl_vec_ut.$(_OBJ) utl_vec_ut.c
	$(CC) -o $@ utl_vec_ut.$(_OBJ)

t_log$(_EXE): $(UTL_H)  utl_logging_ut.c
	$(CC) $(CFLAGS) -c -o utl_logging_ut.$(_OBJ) utl_logging_ut.c
	$(CC) -o $@ utl_logging_ut.$(_OBJ)

t_general$(_EXE): $(UTL_H)  utl_general_ut.c
	$(CC) $(CFLAGS) -c -o utl_general_ut.$(_OBJ) utl_general_ut.c
	$(CC) -o $@ utl_general_ut.$(_OBJ)

t_try$(_EXE): $(UTL_H)  utl_exception_ut.c
	$(CC) $(CFLAGS) -c -o utl_exception_ut.$(_OBJ) utl_exception_ut.c
	$(CC) -o $@ utl_exception_ut.$(_OBJ)

t_try2$(_EXE): $(UTL_H)  utl_exception2_ut.c
	$(CC) $(CFLAGS) -c -o utl_exception2_ut.$(_OBJ) utl_exception2_ut.c
	$(CC) -o $@ utl_exception2_ut.$(_OBJ)

t_mem$(_EXE): $(UTL_H)  utl_memory_ut.c
	$(CC) $(CFLAGS) -c -o utl_memory_ut.$(_OBJ) utl_memory_ut.c
	$(CC) -o $@ utl_memory_ut.$(_OBJ)

t_fsm$(_EXE): $(UTL_H)  utl_fsm_ut.c
	$(CC) $(CFLAGS) -c -o utl_fsm_ut.$(_OBJ) utl_fsm_ut.c
	$(CC) -o $@ utl_fsm_ut.$(_OBJ)

clean:
	rm -f t_*$(_EXE) *.$(_OBJ) *.tmp *.log

