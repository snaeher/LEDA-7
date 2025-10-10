.section __TEXT,__text,regular,pure_instructions
.section __TEXT,__picsymbolstub1,symbol_stubs,pure_instructions,32
.section __TEXT,__text,regular,pure_instructions


	.align 2
	.globl _ieee_get_fcr
.section __TEXT,__text,regular,pure_instructions
	.align 2
_ieee_get_fcr:
	stmw r30,-8(r1)
	stwu r1,-64(r1)
	mr r30,r1
	mffs f1 
        stfd f1,40(r30)
	lwz r0,44(r30)
	stw r0,32(r30)
	lwz r1,0(r1)
	lmw r30,-8(r1)
	blr


	.align 2
	.globl _ieee_set_fcr
.section __TEXT,__text,regular,pure_instructions
	.align 2
_ieee_set_fcr:
	stmw r30,-8(r1)
	stwu r1,-64(r1)
	mr r30,r1
	li r0,243
	stw r0,32(r30)
	lis r0,0xfff8
	stw r0,40(r30)
	lwz r0,32(r30)
	stw r0,44(r30)
	lfd f1,40(r30)
        mtfsf 255,f1
	lwz r1,0(r1)
	lmw r30,-8(r1)
	blr


	.align 2
	.globl _ieee_get_fsr
.section __TEXT,__text,regular,pure_instructions
	.align 2
_ieee_get_fsr:
	stmw r30,-8(r1)
	stwu r1,-64(r1)
	mr r30,r1
	mffs f1 
        stfd f1,40(r30)
	lwz r0,44(r30)
	stw r0,32(r30)
	lwz r1,0(r1)
	lmw r30,-8(r1)
	blr


	.align 2
	.globl _ieee_set_fsr
.section __TEXT,__text,regular,pure_instructions
	.align 2
_ieee_set_fsr:
	stmw r30,-8(r1)
	stwu r1,-64(r1)
	mr r30,r1
	li r0,243
	stw r0,32(r30)
	lis r0,0xfff8
	stw r0,40(r30)
	lwz r0,32(r30)
	stw r0,44(r30)
	lfd f1,40(r30)
        mtfsf 255,f1
	lwz r1,0(r1)
	lmw r30,-8(r1)
	blr

