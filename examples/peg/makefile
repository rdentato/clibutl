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

RM=rm -f
CFLAGS = -O2 -I../../src

all: calculator$(_EXE)

calculator$(_EXE): utl.o calculator.o calculator.o
	$(CC) $(LNFLAGS) -o calculator.exe calculator.o utl.o
  
utl.o: ../../src/utl.c
	$(CC) $(CFLAGS) -c -o utl.o ../../src/utl.c

clean:
	$(RM) utl.o
	$(RM) calculator.o calculator$(_EXE)
