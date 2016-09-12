# 
#  (C) 2009 Remo Dentato (rdentato@gmail.com)
# 
# This software is distributed under the terms of the MIT license:
#   https://opensource.org/licenses/MIT
#

all:  
	cd src; make
	cd test; make
	cd dist; make
  
clean:
	cd src; make clean
	cd test; make clean
	cd dist; make clean
