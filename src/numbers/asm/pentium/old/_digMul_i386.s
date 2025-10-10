	.file	"_digMul_i386.c"
	.version	"01.01"
gcc2_compiled.:
.text
	.align 16
.globl _digMul_i386
	.type	 _digMul_i386,@function
_digMul_i386:
	/* Save stack state */
	pushl %ebp
	movl %esp,%ebp

	movl 16(%ebp),%ecx # ecx := high
	movl 8(%ebp),%eax  # eax := a
	mull 12(%ebp)      # edx:eax := a*b
	movl %edx,(%ecx)   # high = highword(a*b)
	/* lowword(a*b) is now in eax */

	/* reestablish stack state */
	movl %ebp,%esp
	popl %ebp
	ret
.Lfe1:
	.size	 _digMul_i386,.Lfe1-_digMul_i386
	.ident	"GCC: (GNU) 2.7.2.3"
