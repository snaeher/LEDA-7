	.set noat
	.set noreorder
	.arch ev56
.text
	.align 5
	.globl ieee_set_fcr
	.ent ieee_set_fcr
ieee_set_fcr:
	.frame $30,16,$26,0
$ieee_set_fcr..ng:
$LFB1:
	lda $30,-16($30)
$LCFI0:
	.prologue 0
	stq $16,0($30)
	excb; ldt $f0,0($30); mt_fpcr $f0; excb
	lda $30,16($30)
	ret $31,($26),1
$LFE1:
	.end ieee_set_fcr


	.align 5
	.globl ieee_get_fcr
	.ent ieee_get_fcr
ieee_get_fcr:
	.frame $30,16,$26,0
$ieee_get_fcr..ng:
$LFB2:
	lda $30,-16($30)
$LCFI1:
	.prologue 0
	excb; mf_fpcr $f0; stt $f0,0($30); excb
	ldq $0,0($30)
	lda $30,16($30)
	ret $31,($26),1
$LFE2:
	.end ieee_get_fcr



	.align 5
	.globl ieee_set_fsr
	.ent ieee_set_fsr
ieee_set_fsr:
	.frame $30,16,$26,0
$ieee_set_fsr..ng:
$LFB3:
	lda $30,-16($30)
$LCFI2:
	.prologue 0
	stq $16,0($30)
	excb; ldt $f0,0($30); mt_fpcr $f0; excb
	lda $30,16($30)
	ret $31,($26),1
$LFE3:
	.end ieee_set_fsr



	.align 5
	.globl ieee_get_fsr
	.ent ieee_get_fsr
ieee_get_fsr:
	.frame $30,16,$26,0
$ieee_get_fsr..ng:
$LFB4:
	lda $30,-16($30)
$LCFI3:
	.prologue 0
	excb; mf_fpcr $f0; stt $f0,0($30); excb
	ldq $0,0($30)
	lda $30,16($30)
	ret $31,($26),1
$LFE4:
	.end ieee_get_fsr

