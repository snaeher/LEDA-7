	.file	"fpu_functions.c"
	.version	"01.01"
gcc2_compiled.:
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
/APP
	fldcw -2(%ebp)
/NO_APP
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
/APP
	fnstcw -2(%ebp)
/NO_APP
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
/APP
	fnstsw -2(%ebp)
/NO_APP
	movzwl -2(%ebp),%eax
	leave
	ret
.LFE3:
.Lfe3:
	.size	 ieee_get_fsr,.Lfe3-ieee_get_fsr
	.align 4
.globl ieee_round_nearest
	.type	 ieee_round_nearest,@function
ieee_round_nearest:
.LFB4:
	pushl %ebp
.LCFI9:
	movl %esp,%ebp
.LCFI10:
	subl $4,%esp
.LCFI11:
	movw $575,-2(%ebp)
/APP
	fldcw -2(%ebp)
/NO_APP
	leave
	ret
.LFE4:
.Lfe4:
	.size	 ieee_round_nearest,.Lfe4-ieee_round_nearest
	.align 4
.globl ieee_round_up
	.type	 ieee_round_up,@function
ieee_round_up:
.LFB5:
	pushl %ebp
.LCFI12:
	movl %esp,%ebp
.LCFI13:
	subl $4,%esp
.LCFI14:
	movw $2623,-2(%ebp)
/APP
	fldcw -2(%ebp)
/NO_APP
	leave
	ret
.LFE5:
.Lfe5:
	.size	 ieee_round_up,.Lfe5-ieee_round_up
	.align 4
.globl ieee_round_down
	.type	 ieee_round_down,@function
ieee_round_down:
.LFB6:
	pushl %ebp
.LCFI15:
	movl %esp,%ebp
.LCFI16:
	subl $4,%esp
.LCFI17:
	movw $1599,-2(%ebp)
/APP
	fldcw -2(%ebp)
/NO_APP
	leave
	ret
.LFE6:
.Lfe6:
	.size	 ieee_round_down,.Lfe6-ieee_round_down
	.align 4
.globl ieee_round_tozero
	.type	 ieee_round_tozero,@function
ieee_round_tozero:
.LFB7:
	pushl %ebp
.LCFI18:
	movl %esp,%ebp
.LCFI19:
	subl $4,%esp
.LCFI20:
	movw $3647,-2(%ebp)
/APP
	fldcw -2(%ebp)
/NO_APP
	leave
	ret
.LFE7:
.Lfe7:
	.size	 ieee_round_tozero,.Lfe7-ieee_round_tozero
	.align 4
.globl set_rounding
	.type	 set_rounding,@function
set_rounding:
.LFB8:
	pushl %ebp
.LCFI21:
	movl %esp,%ebp
.LCFI22:
	subl $4,%esp
.LCFI23:
	movl 8(%ebp),%eax
/APP
	fnstcw -2(%ebp)
/NO_APP
	movw -2(%ebp),%dx
	andb $243,%dh
	movw %dx,-4(%ebp)
	orw %ax,-4(%ebp)
/APP
	fldcw -4(%ebp)
/NO_APP
	movzwl -2(%ebp),%eax
	leave
	ret
.LFE8:
.Lfe8:
	.size	 set_rounding,.Lfe8-set_rounding
	.align 4
.globl ieee_clear_all_exceptions
	.type	 ieee_clear_all_exceptions,@function
ieee_clear_all_exceptions:
.LFB9:
	pushl %ebp
.LCFI24:
	movl %esp,%ebp
.LCFI25:
	subl $4,%esp
.LCFI26:
/APP
	fnstsw -2(%ebp)
	fclex
/NO_APP
	movzwl -2(%ebp),%eax
	leave
	ret
.LFE9:
.Lfe9:
	.size	 ieee_clear_all_exceptions,.Lfe9-ieee_clear_all_exceptions
	.align 4
.globl ieee_clear_current_exceptions
	.type	 ieee_clear_current_exceptions,@function
ieee_clear_current_exceptions:
.LFB10:
	pushl %ebp
.LCFI27:
	movl %esp,%ebp
.LCFI28:
	xorl %eax,%eax
	leave
	ret
.LFE10:
.Lfe10:
	.size	 ieee_clear_current_exceptions,.Lfe10-ieee_clear_current_exceptions
	.align 4
.globl ieee_any_exceptions_raised
	.type	 ieee_any_exceptions_raised,@function
ieee_any_exceptions_raised:
.LFB11:
	pushl %ebp
.LCFI29:
	movl %esp,%ebp
.LCFI30:
	subl $4,%esp
.LCFI31:
/APP
	fnstsw -2(%ebp)
/NO_APP
	movw -2(%ebp),%ax
	andl $61,%eax
	andl $65535,%eax
	leave
	ret
.LFE11:
.Lfe11:
	.size	 ieee_any_exceptions_raised,.Lfe11-ieee_any_exceptions_raised
	.align 4
.globl ieee_nontrivial_exceptions_raised
	.type	 ieee_nontrivial_exceptions_raised,@function
ieee_nontrivial_exceptions_raised:
.LFB12:
	pushl %ebp
.LCFI32:
	movl %esp,%ebp
.LCFI33:
	subl $4,%esp
.LCFI34:
/APP
	fnstsw -2(%ebp)
/NO_APP
	movw -2(%ebp),%ax
	andl $29,%eax
	andl $65535,%eax
	leave
	ret
.LFE12:
.Lfe12:
	.size	 ieee_nontrivial_exceptions_raised,.Lfe12-ieee_nontrivial_exceptions_raised
	.align 4
.globl exceptions_raised
	.type	 exceptions_raised,@function
exceptions_raised:
.LFB13:
	pushl %ebp
.LCFI35:
	movl %esp,%ebp
.LCFI36:
	subl $4,%esp
.LCFI37:
	movl 8(%ebp),%eax
/APP
	fnstsw -2(%ebp)
/NO_APP
	andw -2(%ebp),%ax
	andl $65535,%eax
	leave
	ret
.LFE13:
.Lfe13:
	.size	 exceptions_raised,.Lfe13-exceptions_raised
	.align 4
.globl ieee_set_defaults
	.type	 ieee_set_defaults,@function
ieee_set_defaults:
.LFB14:
	pushl %ebp
.LCFI38:
	movl %esp,%ebp
.LCFI39:
/APP
	fldcw ieee_default_mask
/NO_APP
	call ieee_clear_all_exceptions
	leave
	ret
.LFE14:
.Lfe14:
	.size	 ieee_set_defaults,.Lfe14-ieee_set_defaults
	.align 4
.globl dbl_add
	.type	 dbl_add,@function
dbl_add:
.LFB15:
	pushl %ebp
.LCFI40:
	movl %esp,%ebp
.LCFI41:
	fldl 8(%ebp)
	faddl 16(%ebp)
	leave
	ret
.LFE15:
.Lfe15:
	.size	 dbl_add,.Lfe15-dbl_add
	.align 4
.globl dbl_sub
	.type	 dbl_sub,@function
dbl_sub:
.LFB16:
	pushl %ebp
.LCFI42:
	movl %esp,%ebp
.LCFI43:
	fldl 8(%ebp)
	fsubl 16(%ebp)
	leave
	ret
.LFE16:
.Lfe16:
	.size	 dbl_sub,.Lfe16-dbl_sub
	.align 4
.globl dbl_mul
	.type	 dbl_mul,@function
dbl_mul:
.LFB17:
	pushl %ebp
.LCFI44:
	movl %esp,%ebp
.LCFI45:
	fldl 8(%ebp)
	fmull 16(%ebp)
	leave
	ret
.LFE17:
.Lfe17:
	.size	 dbl_mul,.Lfe17-dbl_mul
	.align 4
.globl dbl_div
	.type	 dbl_div,@function
dbl_div:
.LFB18:
	pushl %ebp
.LCFI46:
	movl %esp,%ebp
.LCFI47:
	fldl 8(%ebp)
	fdivl 16(%ebp)
	leave
	ret
.LFE18:
.Lfe18:
	.size	 dbl_div,.Lfe18-dbl_div
	.local	ieee_invalid
	.comm	ieee_invalid,2,2
	.local	ieee_divbyzero
	.comm	ieee_divbyzero,2,2
	.local	ieee_overflow
	.comm	ieee_overflow,2,2
	.local	ieee_underflow
	.comm	ieee_underflow,2,2
	.local	ieee_inexact
	.comm	ieee_inexact,2,2
	.align 4
.globl _GLOBAL_.I.ieee_set_fcr
	.type	 _GLOBAL_.I.ieee_set_fcr,@function
_GLOBAL_.I.ieee_set_fcr:
.LFB19:
	pushl %ebp
.LCFI48:
	movl %esp,%ebp
.LCFI49:
	movw ieee_exception_bit+8,%ax
	movw %ax,ieee_inexact
	movw ieee_exception_bit+4,%ax
	movw %ax,ieee_underflow
	movw ieee_exception_bit+2,%ax
	movw %ax,ieee_overflow
	movw ieee_exception_bit+6,%ax
	movw %ax,ieee_divbyzero
	movw ieee_exception_bit,%ax
	movw %ax,ieee_invalid
	leave
	ret
.LFE19:
.Lfe19:
	.size	 _GLOBAL_.I.ieee_set_fcr,.Lfe19-_GLOBAL_.I.ieee_set_fcr
.section	.ctors,"aw"
	.long	 _GLOBAL_.I.ieee_set_fcr
.section	.rodata
	.align 2
	.type	 ieee_exception_bit,@object
	.size	 ieee_exception_bit,10
ieee_exception_bit:
	.value 1
	.value 8
	.value 16
	.value 4
	.value 32
	.align 2
	.type	 ieee_default_mask,@object
	.size	 ieee_default_mask,2
ieee_default_mask:
	.value 575

.section	.eh_frame,"aw",@progbits
__FRAME_BEGIN__:
	.4byte	.LLCIE1
.LSCIE1:
	.4byte	0x0
	.byte	0x1
	.byte	0x0
	.byte	0x1
	.byte	0x7c
	.byte	0x8
	.byte	0xc
	.byte	0x4
	.byte	0x4
	.byte	0x88
	.byte	0x1
	.align 4
.LECIE1:
	.set	.LLCIE1,.LECIE1-.LSCIE1
	.4byte	.LLFDE1
.LSFDE1:
	.4byte	.LSFDE1-__FRAME_BEGIN__
	.4byte	.LFB1
	.4byte	.LFE1-.LFB1
	.byte	0x4
	.4byte	.LCFI0-.LFB1
	.byte	0xe
	.byte	0x8
	.byte	0x85
	.byte	0x2
	.byte	0x4
	.4byte	.LCFI1-.LCFI0
	.byte	0xd
	.byte	0x5
	.align 4
.LEFDE1:
	.set	.LLFDE1,.LEFDE1-.LSFDE1
	.4byte	.LLFDE3
.LSFDE3:
	.4byte	.LSFDE3-__FRAME_BEGIN__
	.4byte	.LFB2
	.4byte	.LFE2-.LFB2
	.byte	0x4
	.4byte	.LCFI3-.LFB2
	.byte	0xe
	.byte	0x8
	.byte	0x85
	.byte	0x2
	.byte	0x4
	.4byte	.LCFI4-.LCFI3
	.byte	0xd
	.byte	0x5
	.align 4
.LEFDE3:
	.set	.LLFDE3,.LEFDE3-.LSFDE3
	.4byte	.LLFDE5
.LSFDE5:
	.4byte	.LSFDE5-__FRAME_BEGIN__
	.4byte	.LFB3
	.4byte	.LFE3-.LFB3
	.byte	0x4
	.4byte	.LCFI6-.LFB3
	.byte	0xe
	.byte	0x8
	.byte	0x85
	.byte	0x2
	.byte	0x4
	.4byte	.LCFI7-.LCFI6
	.byte	0xd
	.byte	0x5
	.align 4
.LEFDE5:
	.set	.LLFDE5,.LEFDE5-.LSFDE5
	.4byte	.LLFDE7
.LSFDE7:
	.4byte	.LSFDE7-__FRAME_BEGIN__
	.4byte	.LFB4
	.4byte	.LFE4-.LFB4
	.byte	0x4
	.4byte	.LCFI9-.LFB4
	.byte	0xe
	.byte	0x8
	.byte	0x85
	.byte	0x2
	.byte	0x4
	.4byte	.LCFI10-.LCFI9
	.byte	0xd
	.byte	0x5
	.align 4
.LEFDE7:
	.set	.LLFDE7,.LEFDE7-.LSFDE7
	.4byte	.LLFDE9
.LSFDE9:
	.4byte	.LSFDE9-__FRAME_BEGIN__
	.4byte	.LFB5
	.4byte	.LFE5-.LFB5
	.byte	0x4
	.4byte	.LCFI12-.LFB5
	.byte	0xe
	.byte	0x8
	.byte	0x85
	.byte	0x2
	.byte	0x4
	.4byte	.LCFI13-.LCFI12
	.byte	0xd
	.byte	0x5
	.align 4
.LEFDE9:
	.set	.LLFDE9,.LEFDE9-.LSFDE9
	.4byte	.LLFDE11
.LSFDE11:
	.4byte	.LSFDE11-__FRAME_BEGIN__
	.4byte	.LFB6
	.4byte	.LFE6-.LFB6
	.byte	0x4
	.4byte	.LCFI15-.LFB6
	.byte	0xe
	.byte	0x8
	.byte	0x85
	.byte	0x2
	.byte	0x4
	.4byte	.LCFI16-.LCFI15
	.byte	0xd
	.byte	0x5
	.align 4
.LEFDE11:
	.set	.LLFDE11,.LEFDE11-.LSFDE11
	.4byte	.LLFDE13
.LSFDE13:
	.4byte	.LSFDE13-__FRAME_BEGIN__
	.4byte	.LFB7
	.4byte	.LFE7-.LFB7
	.byte	0x4
	.4byte	.LCFI18-.LFB7
	.byte	0xe
	.byte	0x8
	.byte	0x85
	.byte	0x2
	.byte	0x4
	.4byte	.LCFI19-.LCFI18
	.byte	0xd
	.byte	0x5
	.align 4
.LEFDE13:
	.set	.LLFDE13,.LEFDE13-.LSFDE13
	.4byte	.LLFDE15
.LSFDE15:
	.4byte	.LSFDE15-__FRAME_BEGIN__
	.4byte	.LFB8
	.4byte	.LFE8-.LFB8
	.byte	0x4
	.4byte	.LCFI21-.LFB8
	.byte	0xe
	.byte	0x8
	.byte	0x85
	.byte	0x2
	.byte	0x4
	.4byte	.LCFI22-.LCFI21
	.byte	0xd
	.byte	0x5
	.align 4
.LEFDE15:
	.set	.LLFDE15,.LEFDE15-.LSFDE15
	.4byte	.LLFDE17
.LSFDE17:
	.4byte	.LSFDE17-__FRAME_BEGIN__
	.4byte	.LFB9
	.4byte	.LFE9-.LFB9
	.byte	0x4
	.4byte	.LCFI24-.LFB9
	.byte	0xe
	.byte	0x8
	.byte	0x85
	.byte	0x2
	.byte	0x4
	.4byte	.LCFI25-.LCFI24
	.byte	0xd
	.byte	0x5
	.align 4
.LEFDE17:
	.set	.LLFDE17,.LEFDE17-.LSFDE17
	.4byte	.LLFDE19
.LSFDE19:
	.4byte	.LSFDE19-__FRAME_BEGIN__
	.4byte	.LFB10
	.4byte	.LFE10-.LFB10
	.byte	0x4
	.4byte	.LCFI27-.LFB10
	.byte	0xe
	.byte	0x8
	.byte	0x85
	.byte	0x2
	.byte	0x4
	.4byte	.LCFI28-.LCFI27
	.byte	0xd
	.byte	0x5
	.align 4
.LEFDE19:
	.set	.LLFDE19,.LEFDE19-.LSFDE19
	.4byte	.LLFDE21
.LSFDE21:
	.4byte	.LSFDE21-__FRAME_BEGIN__
	.4byte	.LFB11
	.4byte	.LFE11-.LFB11
	.byte	0x4
	.4byte	.LCFI29-.LFB11
	.byte	0xe
	.byte	0x8
	.byte	0x85
	.byte	0x2
	.byte	0x4
	.4byte	.LCFI30-.LCFI29
	.byte	0xd
	.byte	0x5
	.align 4
.LEFDE21:
	.set	.LLFDE21,.LEFDE21-.LSFDE21
	.4byte	.LLFDE23
.LSFDE23:
	.4byte	.LSFDE23-__FRAME_BEGIN__
	.4byte	.LFB12
	.4byte	.LFE12-.LFB12
	.byte	0x4
	.4byte	.LCFI32-.LFB12
	.byte	0xe
	.byte	0x8
	.byte	0x85
	.byte	0x2
	.byte	0x4
	.4byte	.LCFI33-.LCFI32
	.byte	0xd
	.byte	0x5
	.align 4
.LEFDE23:
	.set	.LLFDE23,.LEFDE23-.LSFDE23
	.4byte	.LLFDE25
.LSFDE25:
	.4byte	.LSFDE25-__FRAME_BEGIN__
	.4byte	.LFB13
	.4byte	.LFE13-.LFB13
	.byte	0x4
	.4byte	.LCFI35-.LFB13
	.byte	0xe
	.byte	0x8
	.byte	0x85
	.byte	0x2
	.byte	0x4
	.4byte	.LCFI36-.LCFI35
	.byte	0xd
	.byte	0x5
	.align 4
.LEFDE25:
	.set	.LLFDE25,.LEFDE25-.LSFDE25
	.4byte	.LLFDE27
.LSFDE27:
	.4byte	.LSFDE27-__FRAME_BEGIN__
	.4byte	.LFB14
	.4byte	.LFE14-.LFB14
	.byte	0x4
	.4byte	.LCFI38-.LFB14
	.byte	0xe
	.byte	0x8
	.byte	0x85
	.byte	0x2
	.byte	0x4
	.4byte	.LCFI39-.LCFI38
	.byte	0xd
	.byte	0x5
	.align 4
.LEFDE27:
	.set	.LLFDE27,.LEFDE27-.LSFDE27
	.4byte	.LLFDE29
.LSFDE29:
	.4byte	.LSFDE29-__FRAME_BEGIN__
	.4byte	.LFB15
	.4byte	.LFE15-.LFB15
	.byte	0x4
	.4byte	.LCFI40-.LFB15
	.byte	0xe
	.byte	0x8
	.byte	0x85
	.byte	0x2
	.byte	0x4
	.4byte	.LCFI41-.LCFI40
	.byte	0xd
	.byte	0x5
	.align 4
.LEFDE29:
	.set	.LLFDE29,.LEFDE29-.LSFDE29
	.4byte	.LLFDE31
.LSFDE31:
	.4byte	.LSFDE31-__FRAME_BEGIN__
	.4byte	.LFB16
	.4byte	.LFE16-.LFB16
	.byte	0x4
	.4byte	.LCFI42-.LFB16
	.byte	0xe
	.byte	0x8
	.byte	0x85
	.byte	0x2
	.byte	0x4
	.4byte	.LCFI43-.LCFI42
	.byte	0xd
	.byte	0x5
	.align 4
.LEFDE31:
	.set	.LLFDE31,.LEFDE31-.LSFDE31
	.4byte	.LLFDE33
.LSFDE33:
	.4byte	.LSFDE33-__FRAME_BEGIN__
	.4byte	.LFB17
	.4byte	.LFE17-.LFB17
	.byte	0x4
	.4byte	.LCFI44-.LFB17
	.byte	0xe
	.byte	0x8
	.byte	0x85
	.byte	0x2
	.byte	0x4
	.4byte	.LCFI45-.LCFI44
	.byte	0xd
	.byte	0x5
	.align 4
.LEFDE33:
	.set	.LLFDE33,.LEFDE33-.LSFDE33
	.4byte	.LLFDE35
.LSFDE35:
	.4byte	.LSFDE35-__FRAME_BEGIN__
	.4byte	.LFB18
	.4byte	.LFE18-.LFB18
	.byte	0x4
	.4byte	.LCFI46-.LFB18
	.byte	0xe
	.byte	0x8
	.byte	0x85
	.byte	0x2
	.byte	0x4
	.4byte	.LCFI47-.LCFI46
	.byte	0xd
	.byte	0x5
	.align 4
.LEFDE35:
	.set	.LLFDE35,.LEFDE35-.LSFDE35
	.4byte	.LLFDE37
.LSFDE37:
	.4byte	.LSFDE37-__FRAME_BEGIN__
	.4byte	.LFB19
	.4byte	.LFE19-.LFB19
	.byte	0x4
	.4byte	.LCFI48-.LFB19
	.byte	0xe
	.byte	0x8
	.byte	0x85
	.byte	0x2
	.byte	0x4
	.4byte	.LCFI49-.LCFI48
	.byte	0xd
	.byte	0x5
	.align 4
.LEFDE37:
	.set	.LLFDE37,.LEFDE37-.LSFDE37
	.ident	"GCC: (GNU) 2.8.1"
