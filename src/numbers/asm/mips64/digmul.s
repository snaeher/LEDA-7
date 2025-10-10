	.set	noreorder
	.set	noat
	#	/usr/lib32/cmplrs/be
	#ident	"$Source: /LEDA/CVS/leda51_sources/src/numbers/asm/mips64/digmul.s,v $ $Revision: 1.1.1.1 $"
	#ism	1274867300

	#-----------------------------------------------------------
	# Compiling mips64.c (/tmp/ctmB.BAAa004Ut)
	#-----------------------------------------------------------

	#-----------------------------------------------------------
	# Options:
	#-----------------------------------------------------------
	#  Target:R10000, ISA:mips4, Pointer Size:64
	#  -O3	(Optimization level)
	#  -g0	(Debug level)
	#  -m1	(Report warnings)
	#-----------------------------------------------------------

	.file	1	"/KM/usr/burnikel/C++/numbers/BigInt/mips/mips64.c"

	.section .text, 1, 0x00000006, 4, 16
.text:
	.section .text

	# Program Unit: digMul
	.ent	digMul
	.globl	digMul
digMul: 	 # 0x0
	.frame	$sp, 0, $31
.BB1.digMul: 	 # 0x0
 #<freq>
 #<freq> BB:1 frequency = 1.00000 (heuristic)
 #<freq>
	.loc	1 6 4
 #   2  
 #   3  extern "C" digit digMul(digit a, digit b, digit* high)
 #   4  {
 #   5     digit low = a*b;
 #   6     *high=low+1;
	dmultu $4,$5                  	# [0]  
	mflo $2                       	# [11]  
	mfhi $1                 	# [12]  CB!
	.loc	1 7 4
 #   7     return low;
	jr $31                        	# [12]  
	.loc	1 6 4
	sd $1,0($6)                   	# [12]  
	.end	digMul
	.section .text
	nop
	nop
	nop

	# Program Unit: .anon_routine.0
	.ent	.anon_routine.0
.anon_routine.0: 	 # 0x20
	.frame	$sp, 0, $31
.BB1..anon_routine.0: 	 # 0x20
 #<freq>
 #<freq> BB:1 frequency = 1.00000 (heuristic)
 #<freq>
	.loc	1 8 1
 #   8  }
	jr $31                        	# [0]  
	nop                           	# [0]  
	.end	.anon_routine.0
	.section .text
	.align 4
	.gpvalue 30720
