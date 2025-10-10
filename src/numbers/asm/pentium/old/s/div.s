.text 
        .align 16
	.globl	vecDivInnerLoop
	.type	vecDivInnerLoop,@function
vecDivInnerLoop:

        pushl %ebx
        pushl %ecx
        pushl %edx

	movl	16(%esp), %ebx
	movl	20(%esp), %ecx
	movl	24(%esp), %edx
	movd	28(%esp), %mm2

        movl    $0xffffffff,%eax
	movd	%eax,%mm0
	movd	%eax,%mm1
        psllq   $32, %mm1
        psubq   %mm0,%mm1

.LLloop:
	movd	(%ecx), %mm3		
	movd	(%ebx),%mm4		
        paddq   %mm1, %mm4
	pmuludq	%mm2, %mm3
	psubq	%mm3, %mm4		
	paddq	%mm4, %mm0		
	movd	%mm0, (%ebx)		
	psrlq	$32, %mm0
        addl    $4,%ecx
        addl    $4,%ebx
        cmpl    %ecx,%edx     
        jg     .LLloop

	movd	%mm0, %eax
        notl    %eax
	subl	%eax, (%ebx)

        movl $0,%eax
        adcl $0,%eax

	emms
        popl %edx
        popl %ecx
        popl %ebx
	ret


