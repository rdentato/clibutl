#   (C) by Remo Dentato (rdentato@gmail.com)
#  
#  This software is distributed under the terms of the MIT license:
#   https://opensource.org/licenses/MIT
#      
#                         ___   __
#                      __/  /_ /  )
#              ___  __(_   ___)  /
#             /  / /  )/  /  /  /
#            /  (_/  //  (__/  / 
#           (____,__/(_____(__/
#     https://github.com/rdentato/clibutl
#
#  Makefile for Gcc tested on Linux and Msys
#

_EXE=.exe
ifeq "$(COMSPEC)" ""
_EXE=
endif

#CC=gcc
AR=ar -ru
RM=rm -f
CP=cp 

CCFLAGS =  -O2
PRFFLAGS = 
WRNFLAGS = -Wall -pedantic
DBGFLAGS =

CFLAGS  = $(PRFFLAGS) $(WRNFLAGS) $(CCFLAGS) -std=c99 -Isrc/
LNFLAGS = $(PRFFLAGS) -Lsrc/

#              __ __  
#      ____ _ / // /_ 
#     / __ `// // /(_)
#    / /_/ // // /_   
#    \__,_//_//_/(_)          

all: src dist tst

#                            
#       _____ _____ _____ _  
#      / ___// ___// ___/(_) 
#     (__  )/ /   / /__ _    
#    /____//_/    \___/(_)   

HDRS = src/utl_hdr.h src/utl_log.h src/utl_mem.h src/utl_vec.h \
       src/utl_pmx.h src/utl_fsm.h src/utl_end.h

CSRC = src/utl_hdr.c src/utl_log.c src/utl_mem.c src/utl_vec.c \
       src/utl_pmx.c 

SNGL = src/utl_hdr.h src/utl_log.h src/utl_mem.c src/utl_mem.h \
       src/utl_vec.h src/utl_pmx.h src/utl_fsm.h src/utl_hdr.c \
       src/utl_log.c src/utl_vec.c src/utl_pmx.c src/utl_end.h

src: src/libutl.a

src/utl.h: src/utl_unc$(_EXE) $(HDRS)
#	cat $(HDRS) > src/utl.h
	src/utl_unc $(HDRS) > src/utl.h

src/utl.c: src/utl.h $(CSRC)
#	cat $(CSRC) > src/utl.c
	src/utl_unc $(CSRC) > src/utl.c
#	for f in $(CSRC); do echo "#line 1 \"$$f\"" >> src/utl.c; cat $$f >> src/utl.c; done

#src/utl_single.h: src/utl_unc$(_EXE) $(SNGL)
#	src/utl_unc $(SNGL) > src/utl_single.h
  
src/utl_unc$(_EXE): src/utl_unc.o
	$(CC) -o $@ src/utl_unc.o

src/libutl.a:  src/utl.o
	$(AR) $@ src/utl.o

  
#            __ _        __  
#       ____/ /(_)_____ / /_ _  
#      / __  // // ___// __/(_) 
#     / /_/ // /(__  )/ /_ _    
#     \__,_//_//____/ \__/(_)   

dist: src/utl.h src/utl.c 
	$(CP) src/utl.h src/utl.c dist


#       __               __    
#      / /_ ___   _____ / /_ _ 
#     / __// _ \ / ___// __/(_)
#    / /_ /  __/(__  )/ /_ _   
#    \__/ \___//____/ \__/(_)

TESTS = test/x_chk.x test/t_vec$(_EXE)  test/t_buf$(_EXE)  test/t_mem$(_EXE)  \
        test/t_pmx$(_EXE)  test/t_trc$(_EXE) \
        test/t_pmx2$(_EXE) test/t_pmx3$(_EXE) test/t_pmx4$(_EXE) \
        test/t_utf$(_EXE)  test/t_logassert$(_EXE)

tst:  $(TESTS) 

test/x_chk.x: src/utl.h
	$(RM) test/*.o
	echo > test/x_chk.x

test/t_vec$(_EXE): test/x_chk.x src/utl.o  test/ut_vec.o
	$(CC) $(LNFLAGS) -o $@ test/ut_vec.o src/utl.o

test/t_buf$(_EXE): test/x_chk.x src/utl.o test/ut_buf.o
	$(CC) $(LNFLAGS) -o $@ test/ut_buf.o src/utl.o

test/t_pmx$(_EXE): test/x_chk.x src/utl.o test/ut_pmx.o
	$(CC) $(LNFLAGS) -o $@ test/ut_pmx.o src/utl.o
  
test/t_pmx2$(_EXE): test/x_chk.x src/utl.o test/ut_pmx2.o
	$(CC) $(LNFLAGS) -o $@ test/ut_pmx2.o src/utl.o

test/t_pmx3$(_EXE): test/x_chk.x src/utl.o test/ut_pmx3.o
	$(CC) $(LNFLAGS) -o $@ test/ut_pmx3.o src/utl.o

test/t_pmx4$(_EXE): test/x_chk.x src/utl.o test/ut_pmx4.o
	$(CC) $(LNFLAGS) -o $@ test/ut_pmx4.o src/utl.o

test/t_mem$(_EXE): test/x_chk.x src/utl.o test/ut_mem.o
	$(CC) $(LNFLAGS) -o $@ test/ut_mem.o src/utl.o
  
test/t_utf$(_EXE): test/x_chk.x src/utl.o  test/ut_utf.o
	$(CC) $(LNFLAGS) -o $@ test/ut_utf.o src/utl.o

test/t_trc$(_EXE): test/x_chk.x src/utl.o  test/ut_trc.o
	$(CC) $(LNFLAGS) -o $@ test/ut_trc.o src/utl.o

test/t_logassert$(_EXE): test/x_chk.x src/utl.o test/ut_logassert.o
	$(CC) $(LNFLAGS) -o $@ test/ut_logassert.o src/utl.o

# Test using `utl_single.h`
#test/t_mem2$(_EXE): src/utl_single.h  test/ut_mem2.o
#	$(CC) $(LNFLAGS) -o $@ test/ut_mem2.o
#
#test/t_buf2$(_EXE): src/utl_single.h  test/ut_buf2.o
#	$(CC) $(LNFLAGS) -o $@ test/ut_buf2.o

runtest: tst
	@echo "'l_logassert.log' is expected to FAIL and generate a coredump" 
	@cd test; for f in t_*; do ./$$f ; done ; grep -a "#KO:" l_*.log


#              __                      
#       _____ / /___   ____ _ ____   _   
#      / ___// // _ \ / __ `// __ \ (_)  
#     / /__ / //  __// /_/ // / / /_     
#     \___//_/ \___/ \__,_//_/ /_/(_)    

clean:
	cd src;  $(RM) utl.c utl.h utl_single.h libutl.a *.o *.obj *.gc?? utl_unc$(_EXE)
	cd test; $(RM) t_* *.o *.obj *.tmp *.log gmon.out *.gc?? utl.c x_chk.x
	$(RM) *.log
