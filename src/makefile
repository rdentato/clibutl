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
CP=cp 

CCFLAGS =  -O2
PRFFLAGS = 
WRNFLAGS = -Wall -pedantic

CFLAGS =  $(PRFFLAGS) $(WRNFLAGS) $(CCFLAGS) -std=c99 

LIBOBJS = utl_pmx.o utl_vec.o

HDRS = utl_hdr.h utl_log.h utl_mem.h utl_vec.h utl_pmx.h utl_fsm.h utl_end.h
CSRC = utl_hdr.c utl_log.c utl_mem.c utl_vec.c utl_pmx.c

SNGL = utl_hdr.h utl_log.h utl_mem.c utl_mem.h utl_vec.h utl_pmx.h utl_fsm.h utl_hdr.c utl_log.c utl_vec.c utl_pmx.c utl_end.h

all:  libutl.a utl_single.h

utl.h: utl_unc$(_EXE) $(HDRS)
	./utl_unc $(HDRS) > utl.h

utl.c: utl.h $(CSRC)
	cat $(CSRC) > utl.c

utl_single.h: utl_unc$(_EXE) $(SNGL)
	./utl_unc $(SNGL) > utl_single.h
  
utl_unc$(_EXE): utl_unc.o
	$(CC) -o $@ utl_unc.o

libutl.a:  utl.o
	$(AR) $@ utl.o

coverage:
	make clean utl.h utl.c
	make "CFLAGS=-fprofile-arcs -ftest-coverage" libutl.a
  
profile:
	make clean utl.h utl.c
	make "CCFLAGS=-pg" libutl.a
  
dist: libutl.a utl_single.h 
	$(CP) libutl.a utl.h utl.c utl_single.h utl.o ../dist
  
clean:
	$(RM) utl.c utl.h utl_single.h libutl.a *.o *.gc?? utl_unc$(_EXE)
