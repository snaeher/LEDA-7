LROOT = ../../../..

AFLAGS= -P -xarch=v8plus
GFLAGS= -Wa,-P -Wa,-xarch=v8plus

D = asm_sparc 
#L = L
L = leda

include ../../../Make.src

sparc_fpu_functions.o:
	g++ -c -o sparc_fpu_functions.o sparc_fpu_functions.s
