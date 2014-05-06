# 
#  (C) 2009 Remo Dentato (rdentato@gmail.com)
# 
# This software is distributed under the terms of the BSD license:
#   http://opensource.org/licenses/bsd-license.php
#

all: 
	cd src; make

test: all
	cd test; make

clean: 
	cd src; make clean
	cd test; make clean
