	.file	"t.c"
	.text
	.p2align 4,,15
.globl vecAdd_i386
	.type	vecAdd_i386, @function
vecAdd_i386:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	cld
	pushl	%ebx
	pushl	%ebx
	movl	20(%ebp), %edi
	movl	16(%ebp), %ecx
	sall	$2, %edi
	movl	20(%ebp), %edx
	addl	%edi, %ecx
	movl	%edi, %ebx
	movl	%ecx, -20(%ebp)
	movl	8(%ebp), %eax
	movl	24(%ebp), %ecx
	addl	%edi, %eax
	addl	%ecx, %ebx
	movl	%eax, -16(%ebp)
	movl	12(%ebp), %ecx
	movl	%ebx, %edi
	subl	%edx, %ecx
	movl	%eax, %esi
	sall	$2, %ecx
	shrl	$2, %ecx
	rep
	movsl
	movl	20(%ebp), %edx
	negl	%edx
	je	.L16
	.p2align 4,,7
.L8:
	movl	-20(%ebp), %ecx
	movl	(%ecx,%edx,4), %eax
	movl	-16(%ebp), %ecx
	movl	(%ecx,%edx,4), %edi
	addl	%edi, %eax
	movl	%eax, (%ebx,%edx,4)
	incl	%edx
	jne	.L8
.L16:
	movl	carry, %esi
	testl	%esi, %esi
	je	.L9
	movl	(%ebx,%edx,4), %eax
	incl	%eax
	movl	%eax, (%ebx,%edx,4)
	incl	%edx
	testl	%eax, %eax
	jne	.L9
	.p2align 4,,7
.L13:
	movl	(%ebx,%edx,4), %eax
	incl	%eax
	movl	%eax, (%ebx,%edx,4)
	incl	%edx
	testl	%eax, %eax
	je	.L13
	.p2align 4,,7
.L9:
	movl	20(%ebp), %ebx
	addl	%ebx, %edx
	cmpl	12(%ebp), %edx
	jge	.L14
	movl	12(%ebp), %edx
.L14:
	movl	%edx, %eax
	popl	%edx
	popl	%ecx
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
	.size	vecAdd_i386, .-vecAdd_i386
	.ident	"GCC: (GNU) 3.3.1 (SuSE Linux)"
