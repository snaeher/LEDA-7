	.file	"_add_sub.c"
	.version	"01.01"
gcc2_compiled.:
.text
	.align 16
	.type	 do_nothing,@function
do_nothing:
	pushl %ebp
	movl %esp,%ebp
	movl %ebp,%esp
	popl %ebp
	ret
.Lfe1:
	.size	 do_nothing,.Lfe1-do_nothing
.globl memcpy
	.align 16
.globl _vecAdd_i386
	.type	 _vecAdd_i386,@function
_vecAdd_i386:
	pushl %ebp
	movl %esp,%ebp
	subl $20,%esp
	pushl %edi
	pushl %esi
	pushl %ebx
	call .L34
.L34:
	popl %ebx
	addl $_GLOBAL_OFFSET_TABLE_+[.-.L34],%ebx
	movl 24(%ebp),%edi
	movl 20(%ebp),%esi
	shrl $4,%esi
	movl %esi,-4(%ebp)
	movl 20(%ebp),%esi
	andl $15,%esi
	movl %esi,-8(%ebp)
	movl 12(%ebp),%esi
	leal (%edi,%esi,4),%esi
	movl %esi,-12(%ebp)
	movl $16,%eax
	subl -8(%ebp),%eax
	sall $2,%eax
	movl 8(%ebp),%edx
	subl %eax,%edx
	movl 16(%ebp),%ecx
	subl %eax,%ecx
	subl %eax,%edi
	xorl %esi,%esi
#APP
	add  $0xffffffff,%esi
#NO_APP
	movl -8(%ebp),%eax
	subl $1,%eax
	cmpl $14,%eax
	ja .L3
	movl %ebx,%esi
	subl .L19@GOTOFF(%ebx,%eax,4),%esi
	jmp *%esi
	.align 16
	.align 4
	.align 4
.L19:
	.long _GLOBAL_OFFSET_TABLE_+[.-.L18]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L17]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L16]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L15]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L14]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L13]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L12]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L11]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L10]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L9]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L8]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L7]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L6]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L5]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L4]
	.align 16
.L4:
#APP
	movl 4(%edx),%eax
	adcl 4(%ecx),%eax
	movl %eax,4(%edi)
#NO_APP
.L5:
#APP
	movl 8(%edx),%eax
	adcl 8(%ecx),%eax
	movl %eax,8(%edi)
#NO_APP
.L6:
#APP
	movl 12(%edx),%eax
	adcl 12(%ecx),%eax
	movl %eax,12(%edi)
#NO_APP
.L7:
#APP
	movl 16(%edx),%eax
	adcl 16(%ecx),%eax
	movl %eax,16(%edi)
#NO_APP
.L8:
#APP
	movl 20(%edx),%eax
	adcl 20(%ecx),%eax
	movl %eax,20(%edi)
#NO_APP
.L9:
#APP
	movl 24(%edx),%eax
	adcl 24(%ecx),%eax
	movl %eax,24(%edi)
#NO_APP
.L10:
#APP
	movl 28(%edx),%eax
	adcl 28(%ecx),%eax
	movl %eax,28(%edi)
#NO_APP
.L11:
#APP
	movl 32(%edx),%eax
	adcl 32(%ecx),%eax
	movl %eax,32(%edi)
#NO_APP
.L12:
#APP
	movl 36(%edx),%eax
	adcl 36(%ecx),%eax
	movl %eax,36(%edi)
#NO_APP
.L13:
#APP
	movl 40(%edx),%eax
	adcl 40(%ecx),%eax
	movl %eax,40(%edi)
#NO_APP
.L14:
#APP
	movl 44(%edx),%eax
	adcl 44(%ecx),%eax
	movl %eax,44(%edi)
#NO_APP
.L15:
#APP
	movl 48(%edx),%eax
	adcl 48(%ecx),%eax
	movl %eax,48(%edi)
#NO_APP
.L16:
#APP
	movl 52(%edx),%eax
	adcl 52(%ecx),%eax
	movl %eax,52(%edi)
#NO_APP
.L17:
#APP
	movl 56(%edx),%eax
	adcl 56(%ecx),%eax
	movl %eax,56(%edi)
#NO_APP
.L18:
#APP
	movl 60(%edx),%eax
	adcl 60(%ecx),%eax
	movl %eax,60(%edi)
#NO_APP
.L3:
#APP
	movl $0,%esi
#NO_APP
	movl %esi,-20(%ebp)
#APP
	adc  %esi,%esi
#NO_APP
	movl %esi,-20(%ebp)
	addl $64,%edx
	addl $64,%ecx
	addl $64,%edi
	subl $1,-4(%ebp)
	jc .L22
	.align 4
.L23:
	movl -20(%ebp),%esi
#APP
	add  $0xffffffff,%esi
	movl 0(%edx),%eax
	adcl 0(%ecx),%eax
	movl %eax,0(%edi)
	movl 4(%edx),%eax
	adcl 4(%ecx),%eax
	movl %eax,4(%edi)
	movl 8(%edx),%eax
	adcl 8(%ecx),%eax
	movl %eax,8(%edi)
	movl 12(%edx),%eax
	adcl 12(%ecx),%eax
	movl %eax,12(%edi)
	movl 16(%edx),%eax
	adcl 16(%ecx),%eax
	movl %eax,16(%edi)
	movl 20(%edx),%eax
	adcl 20(%ecx),%eax
	movl %eax,20(%edi)
	movl 24(%edx),%eax
	adcl 24(%ecx),%eax
	movl %eax,24(%edi)
	movl 28(%edx),%eax
	adcl 28(%ecx),%eax
	movl %eax,28(%edi)
	movl 32(%edx),%eax
	adcl 32(%ecx),%eax
	movl %eax,32(%edi)
	movl 36(%edx),%eax
	adcl 36(%ecx),%eax
	movl %eax,36(%edi)
	movl 40(%edx),%eax
	adcl 40(%ecx),%eax
	movl %eax,40(%edi)
	movl 44(%edx),%eax
	adcl 44(%ecx),%eax
	movl %eax,44(%edi)
	movl 48(%edx),%eax
	adcl 48(%ecx),%eax
	movl %eax,48(%edi)
	movl 52(%edx),%eax
	adcl 52(%ecx),%eax
	movl %eax,52(%edi)
	movl 56(%edx),%eax
	adcl 56(%ecx),%eax
	movl %eax,56(%edi)
	movl 60(%edx),%eax
	adcl 60(%ecx),%eax
	movl %eax,60(%edi)
	movl $0,%esi
#NO_APP
	movl %esi,-20(%ebp)
#APP
	adc  %esi,%esi
#NO_APP
	movl %esi,-20(%ebp)
	addl $64,%edx
	addl $64,%ecx
	addl $64,%edi
	subl $1,-4(%ebp)
	jnc .L23
.L22:
	cmpl %edx,%edi
	je .L25
	movl 12(%ebp),%esi
	subl 20(%ebp),%esi
	leal 0(,%esi,4),%eax
	pushl %eax
	pushl %edx
	pushl %edi
	call memcpy@PLT
	movl $0,(%edi,%esi,4)
.L25:
	cmpl $0,-20(%ebp)
	je .L26
	jmp .L33
	.align 16
.L29:
	addl $4,%edi
.L33:
	incl (%edi)
	je .L29
	cmpl -12(%ebp),%edi
	jne .L26
	incl 12(%ebp)
.L26:
	movl 12(%ebp),%eax
	leal -32(%ebp),%esp
	popl %ebx
	popl %esi
	popl %edi
	movl %ebp,%esp
	popl %ebp
	ret
.Lfe2:
	.size	 _vecAdd_i386,.Lfe2-_vecAdd_i386
	.align 16
.globl _vecAddTo_i386
	.type	 _vecAddTo_i386,@function
_vecAddTo_i386:
	pushl %ebp
	movl %esp,%ebp
	subl $12,%esp
	pushl %edi
	pushl %esi
	pushl %ebx
	call .L66
.L66:
	popl %ebx
	addl $_GLOBAL_OFFSET_TABLE_+[.-.L66],%ebx
	movl 8(%ebp),%esi
	movl 20(%ebp),%edx
	shrl $4,%edx
	movl %edx,-4(%ebp)
	movl 20(%ebp),%edx
	andl $15,%edx
	movl %edx,-12(%ebp)
	movl 12(%ebp),%edx
	leal (%esi,%edx,4),%edx
	movl %edx,-8(%ebp)
	movl $16,%eax
	subl -12(%ebp),%eax
	sall $2,%eax
	subl %eax,%esi
	movl 16(%ebp),%ecx
	subl %eax,%ecx
	xorl %edi,%edi
#APP
	add  $0xffffffff,%edi
#NO_APP
	movl -12(%ebp),%eax
	subl $1,%eax
	cmpl $14,%eax
	ja .L36
	movl %ebx,%edx
	subl .L52@GOTOFF(%ebx,%eax,4),%edx
	jmp *%edx
	.align 16
	.align 4
	.align 4
.L52:
	.long _GLOBAL_OFFSET_TABLE_+[.-.L51]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L50]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L49]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L48]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L47]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L46]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L45]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L44]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L43]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L42]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L41]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L40]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L39]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L38]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L37]
	.align 16
.L37:
#APP
	movl 4(%esi),%eax
	adcl 4(%ecx),%eax
	movl %eax,4(%esi)
#NO_APP
.L38:
#APP
	movl 8(%esi),%eax
	adcl 8(%ecx),%eax
	movl %eax,8(%esi)
#NO_APP
.L39:
#APP
	movl 12(%esi),%eax
	adcl 12(%ecx),%eax
	movl %eax,12(%esi)
#NO_APP
.L40:
#APP
	movl 16(%esi),%eax
	adcl 16(%ecx),%eax
	movl %eax,16(%esi)
#NO_APP
.L41:
#APP
	movl 20(%esi),%eax
	adcl 20(%ecx),%eax
	movl %eax,20(%esi)
#NO_APP
.L42:
#APP
	movl 24(%esi),%eax
	adcl 24(%ecx),%eax
	movl %eax,24(%esi)
#NO_APP
.L43:
#APP
	movl 28(%esi),%eax
	adcl 28(%ecx),%eax
	movl %eax,28(%esi)
#NO_APP
.L44:
#APP
	movl 32(%esi),%eax
	adcl 32(%ecx),%eax
	movl %eax,32(%esi)
#NO_APP
.L45:
#APP
	movl 36(%esi),%eax
	adcl 36(%ecx),%eax
	movl %eax,36(%esi)
#NO_APP
.L46:
#APP
	movl 40(%esi),%eax
	adcl 40(%ecx),%eax
	movl %eax,40(%esi)
#NO_APP
.L47:
#APP
	movl 44(%esi),%eax
	adcl 44(%ecx),%eax
	movl %eax,44(%esi)
#NO_APP
.L48:
#APP
	movl 48(%esi),%eax
	adcl 48(%ecx),%eax
	movl %eax,48(%esi)
#NO_APP
.L49:
#APP
	movl 52(%esi),%eax
	adcl 52(%ecx),%eax
	movl %eax,52(%esi)
#NO_APP
.L50:
#APP
	movl 56(%esi),%eax
	adcl 56(%ecx),%eax
	movl %eax,56(%esi)
#NO_APP
.L51:
#APP
	movl 60(%esi),%eax
	adcl 60(%ecx),%eax
	movl %eax,60(%esi)
#NO_APP
.L36:
#APP
	movl $0,%edi
	adc  %edi,%edi
#NO_APP
	addl $64,%esi
	addl $64,%ecx
	subl $1,-4(%ebp)
	jc .L55
	.align 4
.L56:
#APP
	add  $0xffffffff,%edi
	movl 0(%esi),%eax
	adcl 0(%ecx),%eax
	movl %eax,0(%esi)
	movl 4(%esi),%eax
	adcl 4(%ecx),%eax
	movl %eax,4(%esi)
	movl 8(%esi),%eax
	adcl 8(%ecx),%eax
	movl %eax,8(%esi)
	movl 12(%esi),%eax
	adcl 12(%ecx),%eax
	movl %eax,12(%esi)
	movl 16(%esi),%eax
	adcl 16(%ecx),%eax
	movl %eax,16(%esi)
	movl 20(%esi),%eax
	adcl 20(%ecx),%eax
	movl %eax,20(%esi)
	movl 24(%esi),%eax
	adcl 24(%ecx),%eax
	movl %eax,24(%esi)
	movl 28(%esi),%eax
	adcl 28(%ecx),%eax
	movl %eax,28(%esi)
	movl 32(%esi),%eax
	adcl 32(%ecx),%eax
	movl %eax,32(%esi)
	movl 36(%esi),%eax
	adcl 36(%ecx),%eax
	movl %eax,36(%esi)
	movl 40(%esi),%eax
	adcl 40(%ecx),%eax
	movl %eax,40(%esi)
	movl 44(%esi),%eax
	adcl 44(%ecx),%eax
	movl %eax,44(%esi)
	movl 48(%esi),%eax
	adcl 48(%ecx),%eax
	movl %eax,48(%esi)
	movl 52(%esi),%eax
	adcl 52(%ecx),%eax
	movl %eax,52(%esi)
	movl 56(%esi),%eax
	adcl 56(%ecx),%eax
	movl %eax,56(%esi)
	movl 60(%esi),%eax
	adcl 60(%ecx),%eax
	movl %eax,60(%esi)
	movl $0,%edi
	adc  %edi,%edi
#NO_APP
	addl $64,%esi
	addl $64,%ecx
	subl $1,-4(%ebp)
	jnc .L56
.L55:
	pushl %ecx
	call do_nothing@PLT
	testl %edi,%edi
	je .L58
	jmp .L65
	.align 16
.L61:
	addl $4,%esi
.L65:
	incl (%esi)
	je .L61
	cmpl -8(%ebp),%esi
	jne .L58
	incl 12(%ebp)
.L58:
	movl 12(%ebp),%eax
	leal -24(%ebp),%esp
	popl %ebx
	popl %esi
	popl %edi
	movl %ebp,%esp
	popl %ebp
	ret
.Lfe3:
	.size	 _vecAddTo_i386,.Lfe3-_vecAddTo_i386
	.align 16
.globl _vecSub_i386
	.type	 _vecSub_i386,@function
_vecSub_i386:
	pushl %ebp
	movl %esp,%ebp
	subl $16,%esp
	pushl %edi
	pushl %esi
	pushl %ebx
	call .L104
.L104:
	popl %ebx
	addl $_GLOBAL_OFFSET_TABLE_+[.-.L104],%ebx
	movl 24(%ebp),%esi
	movl 20(%ebp),%edx
	shrl $4,%edx
	movl %edx,-4(%ebp)
	movl 20(%ebp),%edx
	andl $15,%edx
	movl %edx,-16(%ebp)
	movl 12(%ebp),%edx
	leal (%esi,%edx,4),%edx
	movl %edx,-8(%ebp)
	movl $16,%eax
	subl -16(%ebp),%eax
	sall $2,%eax
	movl 8(%ebp),%edi
	subl %eax,%edi
	movl 16(%ebp),%ecx
	subl %eax,%ecx
	subl %eax,%esi
	xorl %edx,%edx
#APP
	add  $0xffffffff,%edx
#NO_APP
	movl -16(%ebp),%eax
	subl $1,%eax
	cmpl $14,%eax
	ja .L68
	movl %ebx,%edx
	subl .L84@GOTOFF(%ebx,%eax,4),%edx
	jmp *%edx
	.align 16
	.align 4
	.align 4
.L84:
	.long _GLOBAL_OFFSET_TABLE_+[.-.L83]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L82]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L81]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L80]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L79]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L78]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L77]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L76]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L75]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L74]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L73]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L72]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L71]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L70]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L69]
	.align 16
.L69:
#APP
	movl 4(%edi),%eax
	sbbl 4(%ecx),%eax
	movl %eax,4(%esi)
#NO_APP
.L70:
#APP
	movl 8(%edi),%eax
	sbbl 8(%ecx),%eax
	movl %eax,8(%esi)
#NO_APP
.L71:
#APP
	movl 12(%edi),%eax
	sbbl 12(%ecx),%eax
	movl %eax,12(%esi)
#NO_APP
.L72:
#APP
	movl 16(%edi),%eax
	sbbl 16(%ecx),%eax
	movl %eax,16(%esi)
#NO_APP
.L73:
#APP
	movl 20(%edi),%eax
	sbbl 20(%ecx),%eax
	movl %eax,20(%esi)
#NO_APP
.L74:
#APP
	movl 24(%edi),%eax
	sbbl 24(%ecx),%eax
	movl %eax,24(%esi)
#NO_APP
.L75:
#APP
	movl 28(%edi),%eax
	sbbl 28(%ecx),%eax
	movl %eax,28(%esi)
#NO_APP
.L76:
#APP
	movl 32(%edi),%eax
	sbbl 32(%ecx),%eax
	movl %eax,32(%esi)
#NO_APP
.L77:
#APP
	movl 36(%edi),%eax
	sbbl 36(%ecx),%eax
	movl %eax,36(%esi)
#NO_APP
.L78:
#APP
	movl 40(%edi),%eax
	sbbl 40(%ecx),%eax
	movl %eax,40(%esi)
#NO_APP
.L79:
#APP
	movl 44(%edi),%eax
	sbbl 44(%ecx),%eax
	movl %eax,44(%esi)
#NO_APP
.L80:
#APP
	movl 48(%edi),%eax
	sbbl 48(%ecx),%eax
	movl %eax,48(%esi)
#NO_APP
.L81:
#APP
	movl 52(%edi),%eax
	sbbl 52(%ecx),%eax
	movl %eax,52(%esi)
#NO_APP
.L82:
#APP
	movl 56(%edi),%eax
	sbbl 56(%ecx),%eax
	movl %eax,56(%esi)
#NO_APP
.L83:
#APP
	movl 60(%edi),%eax
	sbbl 60(%ecx),%eax
	movl %eax,60(%esi)
#NO_APP
.L68:
#APP
	movl $0,%edx
#NO_APP
	movl %edx,-12(%ebp)
#APP
	adc  %edx,%edx
#NO_APP
	movl %edx,-12(%ebp)
	addl $64,%edi
	addl $64,%ecx
	addl $64,%esi
	subl $1,-4(%ebp)
	jc .L87
	.align 4
.L88:
	movl -12(%ebp),%edx
#APP
	add  $0xffffffff,%edx
	movl 0(%edi),%eax
	sbbl 0(%ecx),%eax
	movl %eax,0(%esi)
	movl 4(%edi),%eax
	sbbl 4(%ecx),%eax
	movl %eax,4(%esi)
	movl 8(%edi),%eax
	sbbl 8(%ecx),%eax
	movl %eax,8(%esi)
	movl 12(%edi),%eax
	sbbl 12(%ecx),%eax
	movl %eax,12(%esi)
	movl 16(%edi),%eax
	sbbl 16(%ecx),%eax
	movl %eax,16(%esi)
	movl 20(%edi),%eax
	sbbl 20(%ecx),%eax
	movl %eax,20(%esi)
	movl 24(%edi),%eax
	sbbl 24(%ecx),%eax
	movl %eax,24(%esi)
	movl 28(%edi),%eax
	sbbl 28(%ecx),%eax
	movl %eax,28(%esi)
	movl 32(%edi),%eax
	sbbl 32(%ecx),%eax
	movl %eax,32(%esi)
	movl 36(%edi),%eax
	sbbl 36(%ecx),%eax
	movl %eax,36(%esi)
	movl 40(%edi),%eax
	sbbl 40(%ecx),%eax
	movl %eax,40(%esi)
	movl 44(%edi),%eax
	sbbl 44(%ecx),%eax
	movl %eax,44(%esi)
	movl 48(%edi),%eax
	sbbl 48(%ecx),%eax
	movl %eax,48(%esi)
	movl 52(%edi),%eax
	sbbl 52(%ecx),%eax
	movl %eax,52(%esi)
	movl 56(%edi),%eax
	sbbl 56(%ecx),%eax
	movl %eax,56(%esi)
	movl 60(%edi),%eax
	sbbl 60(%ecx),%eax
	movl %eax,60(%esi)
	movl $0,%edx
#NO_APP
	movl %edx,-12(%ebp)
#APP
	adc  %edx,%edx
#NO_APP
	movl %edx,-12(%ebp)
	addl $64,%edi
	addl $64,%ecx
	addl $64,%esi
	subl $1,-4(%ebp)
	jnc .L88
.L87:
	pushl %ecx
	call do_nothing@PLT
	addl $4,%esp
	cmpl %edi,%esi
	je .L90
	movl 12(%ebp),%eax
	subl 20(%ebp),%eax
	sall $2,%eax
	pushl %eax
	pushl %edi
	pushl %esi
	call memcpy@PLT
.L90:
	cmpl $0,-12(%ebp)
	je .L103
	jmp .L102
	.align 16
.L94:
	addl $4,%esi
.L102:
	decl (%esi)
	cmpl $-1,(%esi)
	je .L94
	jmp .L103
	.align 16
.L98:
	decl 12(%ebp)
.L103:
	addl $-4,-8(%ebp)
	movl -8(%ebp),%edx
	cmpl $0,(%edx)
	je .L98
	movl 12(%ebp),%eax
	leal -28(%ebp),%esp
	popl %ebx
	popl %esi
	popl %edi
	movl %ebp,%esp
	popl %ebp
	ret
.Lfe4:
	.size	 _vecSub_i386,.Lfe4-_vecSub_i386
	.align 16
.globl _vecSubFrom_i386
	.type	 _vecSubFrom_i386,@function
_vecSubFrom_i386:
	pushl %ebp
	movl %esp,%ebp
	subl $12,%esp
	pushl %edi
	pushl %esi
	pushl %ebx
	call .L141
.L141:
	popl %ebx
	addl $_GLOBAL_OFFSET_TABLE_+[.-.L141],%ebx
	movl 8(%ebp),%esi
	movl 20(%ebp),%edx
	shrl $4,%edx
	movl %edx,-4(%ebp)
	movl 20(%ebp),%edx
	andl $15,%edx
	movl %edx,-12(%ebp)
	movl 12(%ebp),%edx
	leal (%esi,%edx,4),%edx
	movl %edx,-8(%ebp)
	movl $16,%eax
	subl -12(%ebp),%eax
	sall $2,%eax
	subl %eax,%esi
	movl 16(%ebp),%ecx
	subl %eax,%ecx
	xorl %edi,%edi
#APP
	add  $0xffffffff,%edi
#NO_APP
	movl -12(%ebp),%eax
	subl $1,%eax
	cmpl $14,%eax
	ja .L106
	movl %ebx,%edx
	subl .L122@GOTOFF(%ebx,%eax,4),%edx
	jmp *%edx
	.align 16
	.align 4
	.align 4
.L122:
	.long _GLOBAL_OFFSET_TABLE_+[.-.L121]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L120]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L119]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L118]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L117]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L116]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L115]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L114]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L113]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L112]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L111]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L110]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L109]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L108]
	.long _GLOBAL_OFFSET_TABLE_+[.-.L107]
	.align 16
.L107:
#APP
	movl 4(%esi),%eax
	sbbl 4(%ecx),%eax
	movl %eax,4(%esi)
#NO_APP
.L108:
#APP
	movl 8(%esi),%eax
	sbbl 8(%ecx),%eax
	movl %eax,8(%esi)
#NO_APP
.L109:
#APP
	movl 12(%esi),%eax
	sbbl 12(%ecx),%eax
	movl %eax,12(%esi)
#NO_APP
.L110:
#APP
	movl 16(%esi),%eax
	sbbl 16(%ecx),%eax
	movl %eax,16(%esi)
#NO_APP
.L111:
#APP
	movl 20(%esi),%eax
	sbbl 20(%ecx),%eax
	movl %eax,20(%esi)
#NO_APP
.L112:
#APP
	movl 24(%esi),%eax
	sbbl 24(%ecx),%eax
	movl %eax,24(%esi)
#NO_APP
.L113:
#APP
	movl 28(%esi),%eax
	sbbl 28(%ecx),%eax
	movl %eax,28(%esi)
#NO_APP
.L114:
#APP
	movl 32(%esi),%eax
	sbbl 32(%ecx),%eax
	movl %eax,32(%esi)
#NO_APP
.L115:
#APP
	movl 36(%esi),%eax
	sbbl 36(%ecx),%eax
	movl %eax,36(%esi)
#NO_APP
.L116:
#APP
	movl 40(%esi),%eax
	sbbl 40(%ecx),%eax
	movl %eax,40(%esi)
#NO_APP
.L117:
#APP
	movl 44(%esi),%eax
	sbbl 44(%ecx),%eax
	movl %eax,44(%esi)
#NO_APP
.L118:
#APP
	movl 48(%esi),%eax
	sbbl 48(%ecx),%eax
	movl %eax,48(%esi)
#NO_APP
.L119:
#APP
	movl 52(%esi),%eax
	sbbl 52(%ecx),%eax
	movl %eax,52(%esi)
#NO_APP
.L120:
#APP
	movl 56(%esi),%eax
	sbbl 56(%ecx),%eax
	movl %eax,56(%esi)
#NO_APP
.L121:
#APP
	movl 60(%esi),%eax
	sbbl 60(%ecx),%eax
	movl %eax,60(%esi)
#NO_APP
.L106:
#APP
	movl $0,%edi
	adc  %edi,%edi
#NO_APP
	addl $64,%esi
	addl $64,%ecx
	subl $1,-4(%ebp)
	jc .L125
	.align 4
.L126:
#APP
	add  $0xffffffff,%edi
	movl 0(%esi),%eax
	sbbl 0(%ecx),%eax
	movl %eax,0(%esi)
	movl 4(%esi),%eax
	sbbl 4(%ecx),%eax
	movl %eax,4(%esi)
	movl 8(%esi),%eax
	sbbl 8(%ecx),%eax
	movl %eax,8(%esi)
	movl 12(%esi),%eax
	sbbl 12(%ecx),%eax
	movl %eax,12(%esi)
	movl 16(%esi),%eax
	sbbl 16(%ecx),%eax
	movl %eax,16(%esi)
	movl 20(%esi),%eax
	sbbl 20(%ecx),%eax
	movl %eax,20(%esi)
	movl 24(%esi),%eax
	sbbl 24(%ecx),%eax
	movl %eax,24(%esi)
	movl 28(%esi),%eax
	sbbl 28(%ecx),%eax
	movl %eax,28(%esi)
	movl 32(%esi),%eax
	sbbl 32(%ecx),%eax
	movl %eax,32(%esi)
	movl 36(%esi),%eax
	sbbl 36(%ecx),%eax
	movl %eax,36(%esi)
	movl 40(%esi),%eax
	sbbl 40(%ecx),%eax
	movl %eax,40(%esi)
	movl 44(%esi),%eax
	sbbl 44(%ecx),%eax
	movl %eax,44(%esi)
	movl 48(%esi),%eax
	sbbl 48(%ecx),%eax
	movl %eax,48(%esi)
	movl 52(%esi),%eax
	sbbl 52(%ecx),%eax
	movl %eax,52(%esi)
	movl 56(%esi),%eax
	sbbl 56(%ecx),%eax
	movl %eax,56(%esi)
	movl 60(%esi),%eax
	sbbl 60(%ecx),%eax
	movl %eax,60(%esi)
	movl $0,%edi
	adc  %edi,%edi
#NO_APP
	addl $64,%esi
	addl $64,%ecx
	subl $1,-4(%ebp)
	jnc .L126
.L125:
	pushl %ecx
	call do_nothing@PLT
	testl %edi,%edi
	je .L140
	jmp .L139
	.align 16
.L131:
	addl $4,%esi
.L139:
	decl (%esi)
	cmpl $-1,(%esi)
	je .L131
	jmp .L140
	.align 16
.L135:
	decl 12(%ebp)
.L140:
	addl $-4,-8(%ebp)
	movl -8(%ebp),%edx
	cmpl $0,(%edx)
	je .L135
	movl 12(%ebp),%eax
	leal -24(%ebp),%esp
	popl %ebx
	popl %esi
	popl %edi
	movl %ebp,%esp
	popl %ebp
	ret
.Lfe5:
	.size	 _vecSubFrom_i386,.Lfe5-_vecSubFrom_i386
	.ident	"GCC: (GNU) egcs-2.91.60 19981201 (egcs-1.1.1 release)"
