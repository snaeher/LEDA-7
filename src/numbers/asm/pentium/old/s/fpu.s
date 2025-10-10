.text
	.align 4
.globl ieee_set_fcr
.type	 ieee_set_fcr,@function
ieee_set_fcr:
.LFB1:
	pushl %ebp
.LCFI0:
	movl %esp,%ebp
.LCFI1:
	subl $4,%esp
.LCFI2:
	movl 8(%ebp),%eax
	movw %ax,-2(%ebp)
	fldcw -2(%ebp)
	leave
	ret
.LFE1:
.Lfe1:
.size	 ieee_set_fcr,.Lfe1-ieee_set_fcr



	.align 4
.globl ieee_get_fcr
.type	 ieee_get_fcr,@function
ieee_get_fcr:
.LFB2:
	pushl %ebp
.LCFI3:
	movl %esp,%ebp
.LCFI4:
	subl $4,%esp
.LCFI5:
	fnstcw -2(%ebp)
	movzwl -2(%ebp),%eax
	leave
	ret
.LFE2:
.Lfe2:
.size	 ieee_get_fcr,.Lfe2-ieee_get_fcr



	.align 4
.globl ieee_get_fsr
	.type	 ieee_get_fsr,@function
ieee_get_fsr:
.LFB3:
	pushl %ebp
.LCFI6:
	movl %esp,%ebp
.LCFI7:
	subl $4,%esp
.LCFI8:
	fnstsw -2(%ebp)
	movzwl -2(%ebp),%eax
	leave
	ret
.LFE3:
.Lfe3:
.size	 ieee_get_fsr,.Lfe3-ieee_get_fsr


.align 4
.globl i386_clear_all_exceptions
.type    i386_clear_all_exceptions,@function
i386_clear_all_exceptions:
.LFB9:
        pushl %ebp
.LCFI24:
        movl %esp,%ebp
.LCFI25:
        subl $4,%esp
.LCFI26:
        fnstsw -2(%ebp)
        fclex
        movzwl -2(%ebp),%eax
        leave
        ret
.LFE9:
.Lfe9:
        .size    i386_clear_all_exceptions,.Lfe9-i386_clear_all_exceptions


