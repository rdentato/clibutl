# 
#  (C) 2009 Remo Dentato (rdentato@gmail.com)
# 
# This software is distributed under the terms of the BSD license:
#   http://opensource.org/licenses/bsd-license.php
#

_EXE=.exe
_OBJ=o
CC=gcc
_BAT=.sh

UTL_H=../src/utl.h

TESTS = t_buf$(_EXE)     t_vec$(_EXE)  t_log$(_EXE)   \
        t_general$(_EXE) t_try$(_EXE)  t_try2$(_EXE)  \
		t_mem$(_EXE)     t_fsm$(_EXE)  t_nolog$(_EXE) \
		t_pmx$(_EXE)

.SUFFIXES: .c .h $(_OBJ)

CFLAGS= -I../src -g 

.c.o:
	$(CC) $(CFLAGS) -c -o $*.$(_OBJ) $*.c

all: clean $(TESTS)
	chmod +x ./run_tests.sh
	bash ./run_tests.sh
	-sed -n -e '/^# \*\*/p' -e '/^# TOTAL/p' tests.log

t_pmx$(_EXE): $(UTL_H) utl_pmx_ut.c
	$(CC) $(CFLAGS) -c -o utl_pmx_ut.$(_OBJ) utl_pmx_ut.c
	gcc -o $@ utl_pmx_ut.$(_OBJ)
	-./$@

t_buf$(_EXE): $(UTL_H)  utl_buf_ut.c
	$(CC) $(CFLAGS) -c -o utl_buf_ut.$(_OBJ) utl_buf_ut.c
	gcc -o $@ utl_buf_ut.$(_OBJ)

t_vec$(_EXE): $(UTL_H)  utl_vec_ut.c
	$(CC) $(CFLAGS) -c -o utl_vec_ut.$(_OBJ) utl_vec_ut.c
	gcc -o $@ utl_vec_ut.$(_OBJ)

t_log$(_EXE): $(UTL_H)  utl_logging_ut.c
	$(CC) $(CFLAGS) -c -o utl_logging_ut.$(_OBJ) utl_logging_ut.c
	gcc -o $@ utl_logging_ut.$(_OBJ)

t_nolog$(_EXE): $(UTL_H) utl_logging_ut.c
	$(CC) -DUTL_NOLOGGING $(CFLAGS) -c -o utl_logging_ut.$(_OBJ) utl_logging_ut.c
	gcc -DUTL_NOLOGGING -o $@ utl_logging_ut.$(_OBJ)

utl_general_ut.o: $(UTL_H) utl_general_ut.c
t_general$(_EXE): utl_general_ut.o
	gcc -o $@ $<

utl_exception_ut.o: $(UTL_H) utl_exception_ut.c
t_try$(_EXE): utl_exception_ut.o
	gcc -o $@ $<
  
utl_exception2_ut.o: $(UTL_H) utl_exception2_ut.c
t_try2$(_EXE): utl_exception2_ut.o
	gcc -o $@ $<

utl_memory_ut.o: $(UTL_H) utl_memory_ut.c
t_mem$(_EXE): utl_memory_ut.o
	gcc -o $@ $<

utl_fsm_ut.o: $(UTL_H) utl_fsm_ut.c
t_fsm$(_EXE): utl_fsm_ut.o
	gcc -o $@ $<

clean:
	rm -f *.exe *.$(_OBJ) *.tmp *.log

