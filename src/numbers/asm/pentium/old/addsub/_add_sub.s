	.file	"_add_sub.c"
	.intel_syntax
	.text
	.type	do_nothing, @function
do_nothing:
	push	%ebp
	mov	%ebp, %esp
	pop	%ebp
	ret
	.size	do_nothing, .-do_nothing
.globl School_Add
	.type	School_Add, @function
School_Add:
	push	%ebp
	mov	%ebp, %esp
	sub	%esp, 24
	mov	%eax, DWORD PTR [%ebp+20]
	shr	%eax, 4
	mov	DWORD PTR [%ebp-4], %eax
	mov	%eax, DWORD PTR [%ebp+20]
	and	%eax, 15
	mov	DWORD PTR [%ebp-8], %eax
	mov	%eax, DWORD PTR [%ebp+12]
	sal	%eax, 2
	add	%eax, DWORD PTR [%ebp+24]
	mov	DWORD PTR [%ebp-12], %eax
	mov	DWORD PTR [%ebp-20], 0
	mov	%eax, 16
	sub	%eax, DWORD PTR [%ebp-8]
	sal	%eax, 2
	sub	DWORD PTR [%ebp+8], %eax
	mov	%eax, 16
	sub	%eax, DWORD PTR [%ebp-8]
	lea	%edx, [0+%eax*4]
	lea	%eax, [%ebp+16]
	sub	DWORD PTR [%eax], %edx
	mov	%eax, 16
	sub	%eax, DWORD PTR [%ebp-8]
	lea	%edx, [0+%eax*4]
	lea	%eax, [%ebp+24]
	sub	DWORD PTR [%eax], %edx
	mov	%eax, DWORD PTR [%ebp-20]
#APP
	add  $0xffffffff,%eax
#NO_APP
	mov	DWORD PTR [%ebp-20], %eax
	cmp	DWORD PTR [%ebp-8], 15
	ja	.L3
	mov	%eax, DWORD PTR [%ebp-8]
	sal	%eax, 2
	mov	%eax, DWORD PTR .L19[%eax]
	jmp	%eax
	.section	.rodata
	.align 4
	.align 4
.L19:
	.long	.L3
	.long	.L18
	.long	.L17
	.long	.L16
	.long	.L15
	.long	.L14
	.long	.L13
	.long	.L12
	.long	.L11
	.long	.L10
	.long	.L9
	.long	.L8
	.long	.L7
	.long	.L6
	.long	.L5
	.long	.L4
	.text
.L4:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 4(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 4(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,4(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
.L5:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 8(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 8(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,8(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
.L6:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 12(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 12(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,12(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
.L7:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 16(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 16(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,16(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
.L8:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 20(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 20(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,20(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
.L9:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 24(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 24(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,24(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
.L10:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 28(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 28(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,28(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
.L11:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 32(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 32(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,32(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
.L12:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 36(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 36(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,36(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
.L13:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 40(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 40(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,40(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
.L14:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 44(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 44(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,44(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
.L15:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 48(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 48(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,48(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
.L16:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 52(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 52(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,52(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
.L17:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 56(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 56(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,56(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
.L18:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 60(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 60(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,60(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
.L3:
#APP
	movl $0,%eax
#NO_APP
	mov	DWORD PTR [%ebp-20], %eax
	mov	%edx, DWORD PTR [%ebp-20]
	mov	%eax, DWORD PTR [%ebp-20]
#APP
	adc  %edx,%eax
#NO_APP
	mov	DWORD PTR [%ebp-20], %eax
	add	DWORD PTR [%ebp+8], 64
	lea	%eax, [%ebp+16]
	add	DWORD PTR [%eax], 64
	lea	%eax, [%ebp+24]
	add	DWORD PTR [%eax], 64
.L21:
	lea	%eax, [%ebp-4]
	dec	DWORD PTR [%eax]
	cmp	DWORD PTR [%ebp-4], -1
	jne	.L23
	jmp	.L22
.L23:
	mov	%eax, DWORD PTR [%ebp-20]
#APP
	add  $0xffffffff,%eax
#NO_APP
	mov	DWORD PTR [%ebp-20], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 0(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 0(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,0(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 4(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 4(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,4(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 8(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 8(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,8(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 12(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 12(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,12(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 16(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 16(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,16(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 20(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 20(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,20(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 24(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 24(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,24(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 28(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 28(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,28(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 32(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 32(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,32(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 36(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 36(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,36(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 40(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 40(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,40(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 44(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 44(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,44(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 48(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 48(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,48(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 52(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 52(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,52(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 56(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 56(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,56(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 60(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 60(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,60(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
#APP
	movl $0,%eax
#NO_APP
	mov	DWORD PTR [%ebp-20], %eax
	mov	%edx, DWORD PTR [%ebp-20]
	mov	%eax, DWORD PTR [%ebp-20]
#APP
	adc  %edx,%eax
#NO_APP
	mov	DWORD PTR [%ebp-20], %eax
	add	DWORD PTR [%ebp+8], 64
	lea	%eax, [%ebp+16]
	add	DWORD PTR [%eax], 64
	lea	%eax, [%ebp+24]
	add	DWORD PTR [%eax], 64
	jmp	.L21
.L22:
	mov	%eax, DWORD PTR [%ebp+24]
	cmp	%eax, DWORD PTR [%ebp+8]
	je	.L24
	mov	%edx, DWORD PTR [%ebp+20]
	mov	%eax, DWORD PTR [%ebp+12]
	sub	%eax, %edx
	mov	DWORD PTR [%ebp-24], %eax
	sub	%esp, 4
	mov	%eax, DWORD PTR [%ebp-24]
	sal	%eax, 2
	push	%eax
	push	DWORD PTR [%ebp+8]
	push	DWORD PTR [%ebp+24]
	call	memcpy
	add	%esp, 16
	mov	%eax, DWORD PTR [%ebp-24]
	lea	%edx, [0+%eax*4]
	mov	%eax, DWORD PTR [%ebp+24]
	mov	DWORD PTR [%eax+%edx], 0
.L24:
	cmp	DWORD PTR [%ebp-20], 0
	je	.L25
.L26:
	mov	%eax, DWORD PTR [%ebp+24]
	inc	DWORD PTR [%eax]
	cmp	DWORD PTR [%eax], 0
	je	.L28
	jmp	.L27
.L28:
	lea	%eax, [%ebp+24]
	add	DWORD PTR [%eax], 4
	jmp	.L26
.L27:
	mov	%eax, DWORD PTR [%ebp+24]
	cmp	%eax, DWORD PTR [%ebp-12]
	jne	.L25
	lea	%eax, [%ebp+12]
	inc	DWORD PTR [%eax]
.L25:
	mov	%eax, DWORD PTR [%ebp+12]
	leave
	ret
	.size	School_Add, .-School_Add
.globl School_Add_To
	.type	School_Add_To, @function
School_Add_To:
	push	%ebp
	mov	%ebp, %esp
	sub	%esp, 24
	mov	%eax, DWORD PTR [%ebp+20]
	shr	%eax, 4
	mov	DWORD PTR [%ebp-4], %eax
	mov	%eax, DWORD PTR [%ebp+20]
	and	%eax, 15
	mov	DWORD PTR [%ebp-8], %eax
	mov	%eax, DWORD PTR [%ebp+12]
	sal	%eax, 2
	add	%eax, DWORD PTR [%ebp+8]
	mov	DWORD PTR [%ebp-12], %eax
	mov	DWORD PTR [%ebp-20], 0
	mov	%eax, 16
	sub	%eax, DWORD PTR [%ebp-8]
	sal	%eax, 2
	sub	DWORD PTR [%ebp+8], %eax
	mov	%eax, 16
	sub	%eax, DWORD PTR [%ebp-8]
	lea	%edx, [0+%eax*4]
	lea	%eax, [%ebp+16]
	sub	DWORD PTR [%eax], %edx
	mov	%eax, DWORD PTR [%ebp-20]
#APP
	add  $0xffffffff,%eax
#NO_APP
	mov	DWORD PTR [%ebp-20], %eax
	cmp	DWORD PTR [%ebp-8], 15
	ja	.L31
	mov	%eax, DWORD PTR [%ebp-8]
	sal	%eax, 2
	mov	%eax, DWORD PTR .L47[%eax]
	jmp	%eax
	.section	.rodata
	.align 4
	.align 4
.L47:
	.long	.L31
	.long	.L46
	.long	.L45
	.long	.L44
	.long	.L43
	.long	.L42
	.long	.L41
	.long	.L40
	.long	.L39
	.long	.L38
	.long	.L37
	.long	.L36
	.long	.L35
	.long	.L34
	.long	.L33
	.long	.L32
	.text
.L32:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 4(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 4(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,4(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
.L33:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 8(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 8(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,8(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
.L34:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 12(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 12(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,12(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
.L35:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 16(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 16(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,16(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
.L36:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 20(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 20(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,20(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
.L37:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 24(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 24(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,24(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
.L38:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 28(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 28(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,28(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
.L39:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 32(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 32(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,32(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
.L40:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 36(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 36(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,36(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
.L41:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 40(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 40(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,40(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
.L42:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 44(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 44(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,44(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
.L43:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 48(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 48(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,48(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
.L44:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 52(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 52(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,52(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
.L45:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 56(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 56(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,56(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
.L46:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 60(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 60(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,60(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
.L31:
#APP
	movl $0,%eax
#NO_APP
	mov	DWORD PTR [%ebp-20], %eax
	mov	%edx, DWORD PTR [%ebp-20]
	mov	%eax, DWORD PTR [%ebp-20]
#APP
	adc  %edx,%eax
#NO_APP
	mov	DWORD PTR [%ebp-20], %eax
	add	DWORD PTR [%ebp+8], 64
	lea	%eax, [%ebp+16]
	add	DWORD PTR [%eax], 64
.L49:
	lea	%eax, [%ebp-4]
	dec	DWORD PTR [%eax]
	cmp	DWORD PTR [%ebp-4], -1
	jne	.L51
	jmp	.L50
.L51:
	mov	%eax, DWORD PTR [%ebp-20]
#APP
	add  $0xffffffff,%eax
#NO_APP
	mov	DWORD PTR [%ebp-20], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 0(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 0(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,0(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 4(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 4(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,4(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 8(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 8(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,8(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 12(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 12(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,12(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 16(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 16(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,16(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 20(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 20(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,20(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 24(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 24(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,24(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 28(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 28(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,28(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 32(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 32(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,32(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 36(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 36(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,36(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 40(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 40(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,40(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 44(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 44(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,44(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 48(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 48(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,48(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 52(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 52(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,52(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 56(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 56(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,56(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 60(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	adcl 60(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,60(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
#APP
	movl $0,%eax
#NO_APP
	mov	DWORD PTR [%ebp-20], %eax
	mov	%edx, DWORD PTR [%ebp-20]
	mov	%eax, DWORD PTR [%ebp-20]
#APP
	adc  %edx,%eax
#NO_APP
	mov	DWORD PTR [%ebp-20], %eax
	add	DWORD PTR [%ebp+8], 64
	lea	%eax, [%ebp+16]
	add	DWORD PTR [%eax], 64
	jmp	.L49
.L50:
	sub	%esp, 12
	push	DWORD PTR [%ebp+16]
	call	do_nothing
	add	%esp, 16
	cmp	DWORD PTR [%ebp-20], 0
	je	.L52
.L53:
	mov	%eax, DWORD PTR [%ebp+8]
	inc	DWORD PTR [%eax]
	cmp	DWORD PTR [%eax], 0
	je	.L55
	jmp	.L54
.L55:
	add	DWORD PTR [%ebp+8], 4
	jmp	.L53
.L54:
	mov	%eax, DWORD PTR [%ebp+8]
	cmp	%eax, DWORD PTR [%ebp-12]
	jne	.L52
	lea	%eax, [%ebp+12]
	inc	DWORD PTR [%eax]
.L52:
	mov	%eax, DWORD PTR [%ebp+12]
	leave
	ret
	.size	School_Add_To, .-School_Add_To
.globl School_Sub
	.type	School_Sub, @function
School_Sub:
	push	%ebp
	mov	%ebp, %esp
	sub	%esp, 24
	mov	%eax, DWORD PTR [%ebp+20]
	shr	%eax, 4
	mov	DWORD PTR [%ebp-4], %eax
	mov	%eax, DWORD PTR [%ebp+20]
	and	%eax, 15
	mov	DWORD PTR [%ebp-8], %eax
	mov	%eax, DWORD PTR [%ebp+12]
	sal	%eax, 2
	add	%eax, DWORD PTR [%ebp+24]
	mov	DWORD PTR [%ebp-12], %eax
	mov	DWORD PTR [%ebp-20], 0
	mov	%eax, 16
	sub	%eax, DWORD PTR [%ebp-8]
	sal	%eax, 2
	sub	DWORD PTR [%ebp+8], %eax
	mov	%eax, 16
	sub	%eax, DWORD PTR [%ebp-8]
	lea	%edx, [0+%eax*4]
	lea	%eax, [%ebp+16]
	sub	DWORD PTR [%eax], %edx
	mov	%eax, 16
	sub	%eax, DWORD PTR [%ebp-8]
	lea	%edx, [0+%eax*4]
	lea	%eax, [%ebp+24]
	sub	DWORD PTR [%eax], %edx
	mov	%eax, DWORD PTR [%ebp-20]
#APP
	add  $0xffffffff,%eax
#NO_APP
	mov	DWORD PTR [%ebp-20], %eax
	cmp	DWORD PTR [%ebp-8], 15
	ja	.L58
	mov	%eax, DWORD PTR [%ebp-8]
	sal	%eax, 2
	mov	%eax, DWORD PTR .L74[%eax]
	jmp	%eax
	.section	.rodata
	.align 4
	.align 4
.L74:
	.long	.L58
	.long	.L73
	.long	.L72
	.long	.L71
	.long	.L70
	.long	.L69
	.long	.L68
	.long	.L67
	.long	.L66
	.long	.L65
	.long	.L64
	.long	.L63
	.long	.L62
	.long	.L61
	.long	.L60
	.long	.L59
	.text
.L59:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 4(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 4(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,4(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
.L60:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 8(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 8(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,8(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
.L61:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 12(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 12(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,12(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
.L62:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 16(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 16(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,16(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
.L63:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 20(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 20(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,20(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
.L64:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 24(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 24(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,24(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
.L65:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 28(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 28(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,28(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
.L66:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 32(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 32(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,32(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
.L67:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 36(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 36(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,36(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
.L68:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 40(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 40(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,40(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
.L69:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 44(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 44(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,44(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
.L70:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 48(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 48(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,48(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
.L71:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 52(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 52(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,52(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
.L72:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 56(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 56(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,56(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
.L73:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 60(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 60(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,60(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
.L58:
#APP
	movl $0,%eax
#NO_APP
	mov	DWORD PTR [%ebp-20], %eax
	mov	%edx, DWORD PTR [%ebp-20]
	mov	%eax, DWORD PTR [%ebp-20]
#APP
	adc  %edx,%eax
#NO_APP
	mov	DWORD PTR [%ebp-20], %eax
	add	DWORD PTR [%ebp+8], 64
	lea	%eax, [%ebp+16]
	add	DWORD PTR [%eax], 64
	lea	%eax, [%ebp+24]
	add	DWORD PTR [%eax], 64
.L76:
	lea	%eax, [%ebp-4]
	dec	DWORD PTR [%eax]
	cmp	DWORD PTR [%ebp-4], -1
	jne	.L78
	jmp	.L77
.L78:
	mov	%eax, DWORD PTR [%ebp-20]
#APP
	add  $0xffffffff,%eax
#NO_APP
	mov	DWORD PTR [%ebp-20], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 0(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 0(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,0(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 4(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 4(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,4(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 8(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 8(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,8(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 12(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 12(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,12(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 16(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 16(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,16(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 20(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 20(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,20(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 24(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 24(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,24(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 28(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 28(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,28(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 32(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 32(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,32(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 36(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 36(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,36(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 40(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 40(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,40(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 44(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 44(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,44(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 48(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 48(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,48(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 52(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 52(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,52(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 56(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 56(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,56(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 60(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 60(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,60(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+24], %eax
#APP
	movl $0,%eax
#NO_APP
	mov	DWORD PTR [%ebp-20], %eax
	mov	%edx, DWORD PTR [%ebp-20]
	mov	%eax, DWORD PTR [%ebp-20]
#APP
	adc  %edx,%eax
#NO_APP
	mov	DWORD PTR [%ebp-20], %eax
	add	DWORD PTR [%ebp+8], 64
	lea	%eax, [%ebp+16]
	add	DWORD PTR [%eax], 64
	lea	%eax, [%ebp+24]
	add	DWORD PTR [%eax], 64
	jmp	.L76
.L77:
	sub	%esp, 12
	push	DWORD PTR [%ebp+16]
	call	do_nothing
	add	%esp, 16
	mov	%eax, DWORD PTR [%ebp+24]
	cmp	%eax, DWORD PTR [%ebp+8]
	je	.L79
	mov	%edx, DWORD PTR [%ebp+20]
	mov	%eax, DWORD PTR [%ebp+12]
	sub	%eax, %edx
	mov	DWORD PTR [%ebp-24], %eax
	sub	%esp, 4
	mov	%eax, DWORD PTR [%ebp-24]
	sal	%eax, 2
	push	%eax
	push	DWORD PTR [%ebp+8]
	push	DWORD PTR [%ebp+24]
	call	memcpy
	add	%esp, 16
.L79:
	cmp	DWORD PTR [%ebp-20], 0
	je	.L80
.L81:
	mov	%eax, DWORD PTR [%ebp+24]
	dec	DWORD PTR [%eax]
	cmp	DWORD PTR [%eax], -1
	je	.L83
	jmp	.L80
.L83:
	lea	%eax, [%ebp+24]
	add	DWORD PTR [%eax], 4
	jmp	.L81
.L80:
	nop
.L84:
	lea	%eax, [%ebp-12]
	sub	DWORD PTR [%eax], 4
	mov	%eax, DWORD PTR [%ebp-12]
	cmp	DWORD PTR [%eax], 0
	je	.L86
	jmp	.L85
.L86:
	lea	%eax, [%ebp+12]
	dec	DWORD PTR [%eax]
	jmp	.L84
.L85:
	mov	%eax, DWORD PTR [%ebp+12]
	leave
	ret
	.size	School_Sub, .-School_Sub
.globl School_Sub_From
	.type	School_Sub_From, @function
School_Sub_From:
	push	%ebp
	mov	%ebp, %esp
	sub	%esp, 24
	mov	%eax, DWORD PTR [%ebp+20]
	shr	%eax, 4
	mov	DWORD PTR [%ebp-4], %eax
	mov	%eax, DWORD PTR [%ebp+20]
	and	%eax, 15
	mov	DWORD PTR [%ebp-8], %eax
	mov	%eax, DWORD PTR [%ebp+12]
	sal	%eax, 2
	add	%eax, DWORD PTR [%ebp+8]
	mov	DWORD PTR [%ebp-12], %eax
	mov	DWORD PTR [%ebp-20], 0
	mov	%eax, 16
	sub	%eax, DWORD PTR [%ebp-8]
	sal	%eax, 2
	sub	DWORD PTR [%ebp+8], %eax
	mov	%eax, 16
	sub	%eax, DWORD PTR [%ebp-8]
	lea	%edx, [0+%eax*4]
	lea	%eax, [%ebp+16]
	sub	DWORD PTR [%eax], %edx
	mov	%eax, DWORD PTR [%ebp-20]
#APP
	add  $0xffffffff,%eax
#NO_APP
	mov	DWORD PTR [%ebp-20], %eax
	cmp	DWORD PTR [%ebp-8], 15
	ja	.L88
	mov	%eax, DWORD PTR [%ebp-8]
	sal	%eax, 2
	mov	%eax, DWORD PTR .L104[%eax]
	jmp	%eax
	.section	.rodata
	.align 4
	.align 4
.L104:
	.long	.L88
	.long	.L103
	.long	.L102
	.long	.L101
	.long	.L100
	.long	.L99
	.long	.L98
	.long	.L97
	.long	.L96
	.long	.L95
	.long	.L94
	.long	.L93
	.long	.L92
	.long	.L91
	.long	.L90
	.long	.L89
	.text
.L89:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 4(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 4(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,4(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
.L90:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 8(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 8(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,8(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
.L91:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 12(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 12(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,12(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
.L92:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 16(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 16(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,16(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
.L93:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 20(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 20(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,20(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
.L94:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 24(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 24(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,24(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
.L95:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 28(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 28(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,28(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
.L96:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 32(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 32(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,32(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
.L97:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 36(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 36(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,36(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
.L98:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 40(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 40(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,40(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
.L99:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 44(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 44(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,44(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
.L100:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 48(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 48(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,48(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
.L101:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 52(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 52(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,52(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
.L102:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 56(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 56(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,56(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
.L103:
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 60(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 60(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,60(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
.L88:
#APP
	movl $0,%eax
#NO_APP
	mov	DWORD PTR [%ebp-20], %eax
	mov	%edx, DWORD PTR [%ebp-20]
	mov	%eax, DWORD PTR [%ebp-20]
#APP
	adc  %edx,%eax
#NO_APP
	mov	DWORD PTR [%ebp-20], %eax
	add	DWORD PTR [%ebp+8], 64
	lea	%eax, [%ebp+16]
	add	DWORD PTR [%eax], 64
.L106:
	lea	%eax, [%ebp-4]
	dec	DWORD PTR [%eax]
	cmp	DWORD PTR [%ebp-4], -1
	jne	.L108
	jmp	.L107
.L108:
	mov	%eax, DWORD PTR [%ebp-20]
#APP
	add  $0xffffffff,%eax
#NO_APP
	mov	DWORD PTR [%ebp-20], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 0(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 0(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,0(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 4(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 4(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,4(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 8(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 8(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,8(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 12(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 12(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,12(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 16(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 16(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,16(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 20(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 20(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,20(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 24(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 24(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,24(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 28(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 28(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,28(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 32(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 32(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,32(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 36(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 36(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,36(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 40(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 40(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,40(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 44(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 44(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,44(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 48(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 48(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,48(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 52(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 52(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,52(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 56(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 56(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,56(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
	mov	%eax, DWORD PTR [%ebp+8]
#APP
	movl 60(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp+16]
#APP
	sbbl 60(%eax),%eax
#NO_APP
	mov	DWORD PTR [%ebp-16], %eax
	mov	%eax, DWORD PTR [%ebp-16]
#APP
	movl %eax,60(%eax)
#NO_APP
	mov	DWORD PTR [%ebp+8], %eax
#APP
	movl $0,%eax
#NO_APP
	mov	DWORD PTR [%ebp-20], %eax
	mov	%edx, DWORD PTR [%ebp-20]
	mov	%eax, DWORD PTR [%ebp-20]
#APP
	adc  %edx,%eax
#NO_APP
	mov	DWORD PTR [%ebp-20], %eax
	add	DWORD PTR [%ebp+8], 64
	lea	%eax, [%ebp+16]
	add	DWORD PTR [%eax], 64
	jmp	.L106
.L107:
	sub	%esp, 12
	push	DWORD PTR [%ebp+16]
	call	do_nothing
	add	%esp, 16
	cmp	DWORD PTR [%ebp-20], 0
	je	.L109
.L110:
	mov	%eax, DWORD PTR [%ebp+8]
	dec	DWORD PTR [%eax]
	cmp	DWORD PTR [%eax], -1
	je	.L112
	jmp	.L109
.L112:
	add	DWORD PTR [%ebp+8], 4
	jmp	.L110
.L109:
	nop
.L113:
	lea	%eax, [%ebp-12]
	sub	DWORD PTR [%eax], 4
	mov	%eax, DWORD PTR [%ebp-12]
	cmp	DWORD PTR [%eax], 0
	je	.L115
	jmp	.L114
.L115:
	lea	%eax, [%ebp+12]
	dec	DWORD PTR [%eax]
	jmp	.L113
.L114:
	mov	%eax, DWORD PTR [%ebp+12]
	leave
	ret
	.size	School_Sub_From, .-School_Sub_From
	.ident	"GCC: (GNU) 3.3.1 (SuSE Linux)"
