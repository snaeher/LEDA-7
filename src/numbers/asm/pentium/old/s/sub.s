
	.text
	.align	8

	.globl	vecSubCarry
	.type	vecSubCarry,@function
vecSubCarry:
        pushl %ebx
        pushl %ecx
        pushl %edx

        movl    $0,%eax
	movd	%eax, %mm0

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

.VSC_L1:

	movd	(%edx,%eax,4), %mm1
	paddq	%mm0, %mm1
	movd	(%ecx,%eax,4), %mm0
	psubq	%mm1, %mm0

	movd	%mm0, (%ebx,%eax,4)

	psrlq	$63, %mm0

	incl	%eax
	jnz	.VSC_L1

	movd	%mm0, %eax

	emms

        popl %edx
        popl %ecx
        popl %ebx

	ret


