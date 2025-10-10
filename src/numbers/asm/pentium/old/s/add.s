
	.text
	.align	8

	.globl	vecAddCarry
	.type	vecAddCarry,@function
vecAddCarry:

        pushl %ebx
        pushl %ecx
        pushl %edx

        movl    $0,%eax
	movd	%eax, %mm2

	movl	16(%esp), %ebx
	movl	20(%esp), %ecx
	movl	24(%esp), %edx
	movl	28(%esp), %eax

	shll	$2, %eax
        addl    %eax, %ebx
        addl    %eax, %ecx
        addl    %eax, %edx
	shrl	$2, %eax
	negl	%eax			

.VAC_L1:
	movd	(%ecx,%eax,4), %mm0
	movd	(%edx,%eax,4), %mm1
	paddq	%mm0, %mm1
	paddq	%mm1, %mm2

	movd	%mm2, (%ebx,%eax,4)

	psrlq	$32, %mm2

	incl	%eax
	jnz	.VAC_L1

	movd	%mm2, %eax

	emms

        popl %edx
        popl %ecx
        popl %ebx

	ret



	.globl	vecAddCarryC16
	.type	vecAddCarryC16,@function
vecAddCarryC16:

        pushl %ebx
        pushl %ecx
        pushl %edx


	movl	16(%esp), %ebx
	movl	20(%esp), %ecx
	movl	24(%esp), %edx
	movl	28(%esp), %eax

	movd	%eax, %mm2


	movd	0(%ecx), %mm0
	movd	0(%edx), %mm1
	paddq	%mm0, %mm1
	paddq	%mm1, %mm2
	movd	%mm2, 0(%ebx)
	psrlq	$32, %mm2

	movd	4(%ecx), %mm0
	movd	4(%edx), %mm1
	paddq	%mm0, %mm1
	paddq	%mm1, %mm2
	movd	%mm2, 4(%ebx)
	psrlq	$32, %mm2

	movd	8(%ecx), %mm0
	movd	8(%edx), %mm1
	paddq	%mm0, %mm1
	paddq	%mm1, %mm2
	movd	%mm2, 8(%ebx)
	psrlq	$32, %mm2

	movd	12(%ecx), %mm0
	movd	12(%edx), %mm1
	paddq	%mm0, %mm1
	paddq	%mm1, %mm2
	movd	%mm2, 12(%ebx)
	psrlq	$32, %mm2

	movd	16(%ecx), %mm0
	movd	16(%edx), %mm1
	paddq	%mm0, %mm1
	paddq	%mm1, %mm2
	movd	%mm2, 16(%ebx)
	psrlq	$32, %mm2

	movd	20(%ecx), %mm0
	movd	20(%edx), %mm1
	paddq	%mm0, %mm1
	paddq	%mm1, %mm2
	movd	%mm2, 20(%ebx)
	psrlq	$32, %mm2

	movd	24(%ecx), %mm0
	movd	24(%edx), %mm1
	paddq	%mm0, %mm1
	paddq	%mm1, %mm2
	movd	%mm2, 24(%ebx)
	psrlq	$32, %mm2

	movd	28(%ecx), %mm0
	movd	28(%edx), %mm1
	paddq	%mm0, %mm1
	paddq	%mm1, %mm2
	movd	%mm2, 28(%ebx)
	psrlq	$32, %mm2

	movd	32(%ecx), %mm0
	movd	32(%edx), %mm1
	paddq	%mm0, %mm1
	paddq	%mm1, %mm2
	movd	%mm2, 32(%ebx)
	psrlq	$32, %mm2

	movd	36(%ecx), %mm0
	movd	36(%edx), %mm1
	paddq	%mm0, %mm1
	paddq	%mm1, %mm2
	movd	%mm2, 36(%ebx)
	psrlq	$32, %mm2

	movd	40(%ecx), %mm0
	movd	40(%edx), %mm1
	paddq	%mm0, %mm1
	paddq	%mm1, %mm2
	movd	%mm2, 40(%ebx)
	psrlq	$32, %mm2

	movd	44(%ecx), %mm0
	movd	44(%edx), %mm1
	paddq	%mm0, %mm1
	paddq	%mm1, %mm2
	movd	%mm2, 44(%ebx)
	psrlq	$32, %mm2

	movd	48(%ecx), %mm0
	movd	48(%edx), %mm1
	paddq	%mm0, %mm1
	paddq	%mm1, %mm2
	movd	%mm2, 48(%ebx)
	psrlq	$32, %mm2

	movd	52(%ecx), %mm0
	movd	52(%edx), %mm1
	paddq	%mm0, %mm1
	paddq	%mm1, %mm2
	movd	%mm2, 52(%ebx)
	psrlq	$32, %mm2

	movd	56(%ecx), %mm0
	movd	56(%edx), %mm1
	paddq	%mm0, %mm1
	paddq	%mm1, %mm2
	movd	%mm2, 56(%ebx)
	psrlq	$32, %mm2

	movd	60(%ecx), %mm0
	movd	60(%edx), %mm1
	paddq	%mm0, %mm1
	paddq	%mm1, %mm2
	movd	%mm2, 60(%ebx)
	psrlq	$32, %mm2

	movd	%mm2, %eax

	emms

        popl %edx
        popl %ecx
        popl %ebx

	ret

