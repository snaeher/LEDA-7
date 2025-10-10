LROOT = ../../..

D = svgalib
#L = W
L = leda

force: 
	@touch *.cpp
	@$(MAKE) all

include ../../Make.src

#CFLAGS = -O2 -Wno-sign-compare -Wno-write-strings

install: force
	-cp ../../../libleda.so ~/demos/app
