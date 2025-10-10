	.file	1 "alpha.c"
	.set noat
	.set noreorder
.text
	.align 5
	.globl digMul
	.ent digMul
digMul:
	.frame $30,0,$26,0
$digMul..ng:
	.prologue 0
	umulh $16,$17,$0
	stq $0,0($18)
	mulq $16,$17,$0
	ret $31,($26),1
	.end digMul
	.ident	"GCC: (GNU) egcs-2.91.60 Debian 2.1 (egcs-1.1.1 release)"
