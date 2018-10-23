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
SOCKLIB=-lws2_32

ifeq "$(COMSPEC)" ""
_EXE=
SOCKLIB=
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
LNFLAGS = $(PRFFLAGS) $(LDFLAGS) -Lsrc/

#              __ __  
#      ____ _ / // /_ 
#     / __ `// // /(_)
#    / /_/ // // /_   
#    \__,_//_//_/(_)          

all: src dist tst

mini: src_mini dist_mini

#                            
#       _____ _____ _____ _  
#      / ___// ___// ___/(_) 
#     (__  )/ /   / /__ _    
#    /____//_/    \___/(_)   

HDRS = src/utl_hdr.h src/utl_log.h src/utl_mem.h src/utl_vec.h \
       src/utl_pmx.h src/utl_peg.h src/utl_fsm.h \
       src/utl_try.h src/utl_end.h
#src/utl_net.h 

CSRC = src/utl_hdr.c src/utl_utf.c src/utl_log.c src/utl_mem.c \
       src/utl_vec.c src/utl_pmx.c src/utl_peg.c 

      # src/utl_net.c

#src/utl_mtr.h
#src/utl_mtr.c 

HDRS_MINI = src/utl_hdr.h src/utl_log.h src/utl_fsm.h \
            src/utl_try.h src/utl_end.h 

CSRC_MINI = src/utl_hdr.c src/utl_log.c 

src: src/libutl.a
#  src/utlnet.o

src/utl.h: src/utl_unc$(_EXE) $(HDRS)
#	cat $(HDRS) > src/utl.h
	src/utl_unc $(HDRS) > src/utl.h

src/utl.c: src/utl.h $(CSRC)
#	cat $(CSRC) > src/utl.c
	src/utl_unc $(CSRC) > src/utl.c
#	for f in $(CSRC); do echo "#line 1 \"$$f\"" >> src/utl.c; cat $$f >> src/utl.c; done

src/utl_unc$(_EXE): src/utl_unc.o
	$(CC) $(LNFLAGS) -o $@ src/utl_unc.o

src/utlnet.o: src/utl.c
	$(CC) $(CFLAGS) -DUTL_NET -c -o src/utlnet.o src/utl.c
  
src/libutl.a:  src/utl.o
	$(AR) $@ src/utl.o

src_mini: src/utl_m.c src/utl_m.h

src/utl_m.h: src/utl_unc$(_EXE) $(HDRS_MINI)
	src/utl_unc $(HDRS_MINI) > src/utl_m.h

src/utl_m.c: src/utl_m.h $(CSRC_MINI)
	src/utl_unc $(CSRC_MINI) > src/utl_m.c

  
#            __ _        __  
#       ____/ /(_)_____ / /_ _  
#      / __  // // ___// __/(_) 
#     / /_/ // /(__  )/ /_ _    
#     \__,_//_//____/ \__/(_)   

dist: src/utl.h src/utl.c 
	$(CP) src/utl.h src/utl.c dist

dist_mini: src/utl_m.h src/utl_m.c 
	$(CP) src/utl_m.h src/utl_m.c dist


#       __               __    
#      / /_ ___   _____ / /_ _ 
#     / __// _ \ / ___// __/(_)
#    / /_ /  __/(__  )/ /_ _   
#    \__/ \___//____/ \__/(_)

TESTS = test/x_chk.x test/t_vec$(_EXE)  test/t_buf$(_EXE)  test/t_mem$(_EXE)  \
        test/t_pmx$(_EXE)  test/t_trc$(_EXE) test/t_vec2$(_EXE) test/t_dpq$(_EXE) \
        test/t_pmx2$(_EXE) test/t_pmx3$(_EXE) test/t_pmx4$(_EXE) test/t_pmx5$(_EXE) \
        test/t_utf$(_EXE)  test/t_logassert$(_EXE) test/t_try$(_EXE) test/t_log$(_EXE) \
        test/t_vec3$(_EXE) test/t_sym$(_EXE) test/t_peg$(_EXE) test/t_peg2$(_EXE) \
        test/t_arb$(_EXE) 

			# test/tt_srv$(_EXE) 
        
# test/t_mtr$(_EXE)

tst:  $(TESTS) 

test/x_chk.x: src/utl.h
	$(RM) test/*.o
	echo > test/x_chk.x

test/t_sym$(_EXE): test/x_chk.x src/utl.o  test/ut_sym.o
	$(CC) $(LNFLAGS) -o $@ test/ut_sym.o src/utl.o

test/t_vec$(_EXE): test/x_chk.x src/utl.o  test/ut_vec.o
	$(CC) $(LNFLAGS) -o $@ test/ut_vec.o src/utl.o

test/t_vec2$(_EXE): test/x_chk.x src/utl.o  test/ut_vec2.o
	$(CC) $(LNFLAGS) -o $@ test/ut_vec2.o src/utl.o

test/t_vec3$(_EXE): test/x_chk.x src/utl.o  test/ut_vec3.o
	$(CC) $(LNFLAGS) -o $@ test/ut_vec3.o src/utl.o

test/t_dpq$(_EXE): test/x_chk.x src/utl.o  test/ut_dpq.o
	$(CC) $(LNFLAGS) -o $@ test/ut_dpq.o src/utl.o

test/t_try$(_EXE): test/x_chk.x src/utl.o  test/ut_try.o
	$(CC) $(LNFLAGS) -o $@ test/ut_try.o src/utl.o

test/t_log$(_EXE): test/x_chk.x src/utl.o  test/ut_log.o
	$(CC) $(LNFLAGS) -o $@ test/ut_log.o src/utl.o

test/t_buf$(_EXE): test/x_chk.x src/utl.o test/ut_buf.o
	$(CC) $(LNFLAGS) -o $@ test/ut_buf.o src/utl.o

test/t_peg$(_EXE): test/x_chk.x src/utl.o test/ut_peg.o
	$(CC) $(LNFLAGS) -o $@ test/ut_peg.o src/utl.o
  
test/t_peg2$(_EXE): test/x_chk.x src/utl.o test/ut_peg2.o
	$(CC) $(LNFLAGS) -o $@ test/ut_peg2.o src/utl.o
  
test/t_peg3$(_EXE): test/x_chk.x src/utl.o test/ut_peg3.o
	$(CC) $(LNFLAGS) -o $@ test/ut_peg3.o src/utl.o
  
test/t_pmx$(_EXE): test/x_chk.x src/utl.o test/ut_pmx.o
	$(CC) $(LNFLAGS) -o $@ test/ut_pmx.o src/utl.o
  
test/t_pmx2$(_EXE): test/x_chk.x src/utl.o test/ut_pmx2.o
	$(CC) $(LNFLAGS) -o $@ test/ut_pmx2.o src/utl.o

test/t_pmx3$(_EXE): test/x_chk.x src/utl.o test/ut_pmx3.o
	$(CC) $(LNFLAGS) -o $@ test/ut_pmx3.o src/utl.o

test/t_pmx4$(_EXE): test/x_chk.x src/utl.o test/ut_pmx4.o
	$(CC) $(LNFLAGS) -o $@ test/ut_pmx4.o src/utl.o

test/t_pmx5$(_EXE): test/x_chk.x src/utl.o test/ut_pmx5.o
	$(CC) $(LNFLAGS) -o $@ test/ut_pmx5.o src/utl.o

test/t_mem$(_EXE): test/x_chk.x src/utl.o test/ut_mem.o
	$(CC) $(LNFLAGS) -o $@ test/ut_mem.o src/utl.o
  
test/t_mpl$(_EXE): test/x_chk.x src/utl.o test/ut_mpl.o
	$(CC) $(LNFLAGS) -o $@ test/ut_mpl.o src/utl.o
  
test/t_mpl2$(_EXE): test/x_chk.x src/utl.o test/ut_mpl2.o
	$(CC) $(LNFLAGS) -o $@ test/ut_mpl2.o src/utl.o
  
test/t_utf$(_EXE): test/x_chk.x src/utl.o  test/ut_utf.o
	$(CC) $(LNFLAGS) -o $@ test/ut_utf.o src/utl.o

test/t_arb$(_EXE): test/x_chk.x src/utl.o  test/ut_arb.o
	$(CC) $(LNFLAGS) -o $@ test/ut_arb.o src/utl.o

test/t_trc$(_EXE): test/x_chk.x src/utl.o  test/ut_trc.o
	$(CC) $(LNFLAGS) -o $@ test/ut_trc.o src/utl.o

test/t_logassert$(_EXE): test/x_chk.x src/utl.o test/ut_logassert.o
	$(CC) $(LNFLAGS) -o $@ test/ut_logassert.o src/utl.o

test/tt_srv$(_EXE): test/x_chk.x src/utlnet.o  test/ut_srv.o
	$(CC) $(LNFLAGS) -o $@ test/ut_srv.o src/utlnet.o $(SOCKLIB)

# Test using `utl_single.h`
#test/t_mem2$(_EXE): src/utl_single.h  test/ut_mem2.o
#	$(CC) $(LNFLAGS) -o $@ test/ut_mem2.o
#
#test/t_buf2$(_EXE): src/utl_single.h  test/ut_buf2.o
#	$(CC) $(LNFLAGS) -o $@ test/ut_buf2.o

runtest: tst
	@echo "'l_logassert' is expected to abort and generate a coredump"
	@echo ""  
	@cd test; for f in t_*; do ./$$f ; done ; grep -a "#KO:" l_*.log


#              __                      
#       _____ / /___   ____ _ ____   _   
#      / ___// // _ \ / __ `// __ \ (_)  
#     / /__ / //  __// /_/ // / / /_     
#     \___//_/ \___/ \__,_//_/ /_/(_)    

clean:
	cd src;  $(RM) utl.c utl.h l_* libutl.a *.o *.obj *.gc?? utl_unc *.exe
	cd test; $(RM) t_* tt_* *.o *.obj *.tmp l_* gmon.out *.gc?? utl.c x_chk.x *.exe
	$(RM) l_*

gcov:
	make "CCFLAGS=-O0 -coverage" "LDFLAGS=--coverage -lgcov"
  
debug:
	make CCFLAGS=-g
