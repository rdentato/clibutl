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
CFLAGS = -std=c99 -O2 -Wall -pedantic -I../../src

all: tbc$(_EXE) 

tbc$(_EXE): utl.o tbc.o
	$(CC) $(LNFLAGS) -o tbc.exe tbc.o utl.o
  
utl.o: ../../src/utl.c
	$(CC) $(CFLAGS) -c -o utl.o ../../src/utl.c

clean:
	$(RM) utl.o
	$(RM) tbc.o tbc$(_EXE)
