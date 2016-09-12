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

CFLAGS = -O2

LIBOBJS = utl_pmx.o utl_vec.o

HDRS = utl_hdr.h utl_log.h utl_mem.h utl_vec.h utl_pmx.h utl_fsm.h utl_end.h
CSRC = utl_hdr.c utl_log.c utl_mem.c utl_vec.c utl_pmx.c

all:  libutl.a

utl.h: $(HDRS)
	cat $(HDRS) > utl.h

utl.c: $(CSRC)
	cat $(CSRC) > utl.c

libutl.a:  utl.h utl.o
	$(AR) $@ utl.o

clean:
	$(RM) utl.c utl.h libutl.a *.o
