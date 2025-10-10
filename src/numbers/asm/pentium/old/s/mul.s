.text
.align 16 
.globl digMul
digMul:
	movl 12(%esp),%ecx
	movl 4(%esp),%eax
	mull  8(%esp)
	movl %edx,(%ecx)
	ret


	.globl	vecMulLoopFirst
	.type	vecMulLoopFirst,@function
vecMulLoopFirst:

        pushl %ebx
        pushl %ecx

        movl    $0,%eax
	movd	%eax, %mm0

	movl	12(%esp), %ebx
	movl	16(%esp), %ecx
	movl	20(%esp), %eax
	movd	24(%esp), %mm1

.Lloop:
	movd	(%ecx), %mm2		
        addl    $4,%ecx
	pmuludq	%mm1, %mm2
	paddq	%mm2, %mm0		
	movd	%mm0, (%ebx)		
	psrlq	$32, %mm0
        addl    $4,%ebx
        cmpl    %ecx,%eax
        jg     .Lloop

	movd	%mm0, %eax
	movl	%eax, (%ebx)

	emms
        popl %ecx
        popl %ebx
	ret


	.globl	vecMulInnerLoop
	.type	vecMulInnerLoop,@function
vecMulInnerLoop:

        pushl %ebx
        pushl %ecx

        movl    $0,%eax
	movd	%eax, %mm0

	movl	12(%esp), %ebx
	movl	16(%esp), %ecx
	movl	20(%esp), %eax
	movd	24(%esp), %mm1

.LLloop:
	movd	(%ecx), %mm2		
        addl    $4,%ecx
	movd	(%ebx),%mm3		
	pmuludq	%mm1, %mm2
	paddq	%mm2, %mm3		
	paddq	%mm3, %mm0		
	movd	%mm0, (%ebx)		
	psrlq	$32, %mm0
        addl    $4,%ebx
        cmpl    %ecx,%eax
        jg     .LLloop

	movd	%mm0, %eax
	movl	%eax, (%ebx)

	emms
        popl %ecx
        popl %ebx
	ret

