LROOT = ../../../..

AFLAGS= -xarch=v9
GFLAGS= -Wa,$(AFLAGS)

D = asm_sparc 
#L = L
L = leda

include ../../../Make.src

sparc_fpu_functions.o:
	g++ -c -o sparc_fpu_functions.o sparc_fpu_functions.s
