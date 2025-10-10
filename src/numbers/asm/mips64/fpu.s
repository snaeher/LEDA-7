	.option pic2
	.section	.text
	.text
	.align	2
.globl	ieee_set_fcr
.ent	ieee_set_fcr
ieee_set_fcr:
	.frame	$sp,48,$31		# vars= 16, regs= 1/0, args= 0, extra= 16
	.mask	0x10000000,-16
	.fmask	0x00000000,0
	dsubu	$sp,$sp,48
	sd	$28,32($sp)
	.set	noat
	lui	$1,%hi(%neg(%gp_rel(ieee_set_fcr)))
	addiu	$1,$1,%lo(%neg(%gp_rel(ieee_set_fcr)))
	daddu	$gp,$1,$25
	.set	at
	sw	$4,16($sp)
 #APP
	lw $8,16($sp); ctc1 $8 $31
 #NO_APP
	ld	$28,32($sp)
	#nop
	.set	noreorder
	.set	nomacro
	j	$31
	daddu	$sp,$sp,48
	.set	macro
	.set	reorder

	.end	ieee_set_fcr


	.align	2
.globl	ieee_get_fcr
.ent	ieee_get_fcr
ieee_get_fcr:
	.frame	$sp,48,$31		# vars= 16, regs= 1/0, args= 0, extra= 16
	.mask	0x10000000,-16
	.fmask	0x00000000,0
	dsubu	$sp,$sp,48
	sd	$28,32($sp)
	.set	noat
	lui	$1,%hi(%neg(%gp_rel(ieee_get_fcr)))
	addiu	$1,$1,%lo(%neg(%gp_rel(ieee_get_fcr)))
	daddu	$gp,$1,$25
	.set	at
 #APP
	cfc1 $8 $31; sw $8,16($sp)
 #NO_APP
	lw	$2,16($sp)
	#nop
	ld	$28,32($sp)
	#nop
	.set	noreorder
	.set	nomacro
	j	$31
	daddu	$sp,$sp,48
	.set	macro
	.set	reorder

	.end	ieee_get_fcr


	.align	2
.globl	ieee_set_fsr
.ent	ieee_set_fsr
ieee_set_fsr:
	.frame	$sp,48,$31		# vars= 16, regs= 1/0, args= 0, extra= 16
	.mask	0x10000000,-16
	.fmask	0x00000000,0
	dsubu	$sp,$sp,48
	sd	$28,32($sp)
	.set	noat
	lui	$1,%hi(%neg(%gp_rel(ieee_set_fsr)))
	addiu	$1,$1,%lo(%neg(%gp_rel(ieee_set_fsr)))
	daddu	$gp,$1,$25
	.set	at
	sw	$4,16($sp)
 #APP
	cfc1 $8 $31; lw $8,16($sp); ctc1 $8 $31
 #NO_APP
	ld	$28,32($sp)
	#nop
	.set	noreorder
	.set	nomacro
	j	$31
	daddu	$sp,$sp,48
	.set	macro
	.set	reorder

	.end	ieee_set_fsr


.align	2
.globl	ieee_get_fsr
.ent	ieee_get_fsr
ieee_get_fsr:
	.frame	$sp,48,$31		# vars= 16, regs= 1/0, args= 0, extra= 16
	.mask	0x10000000,-16
	.fmask	0x00000000,0
	dsubu	$sp,$sp,48
	sd	$28,32($sp)
	.set	noat
	lui	$1,%hi(%neg(%gp_rel(ieee_get_fsr)))
	addiu	$1,$1,%lo(%neg(%gp_rel(ieee_get_fsr)))
	daddu	$gp,$1,$25
	.set	at
 #APP
	cfc1 $8 $31; sw $8,16($sp)
 #NO_APP
	lw	$2,16($sp)
	#nop
	ld	$28,32($sp)
	#nop
	.set	noreorder
	.set	nomacro
	j	$31
	daddu	$sp,$sp,48
	.set	macro
	.set	reorder

	.end	ieee_get_fsr

