	.file	"_add_sub.c"
gcc2_compiled.:
.section	".text"
	.align 4
	.type	 do_nothing,#function
	.proc	020
do_nothing:
	!#PROLOGUE# 0
	retl
	nop
.LLfe1:
	.size	 do_nothing,.LLfe1-do_nothing
	.global memcpy
	.align 4
	.global _vecAdd_v8
	.type	 _vecAdd_v8,#function
	.proc	016
_vecAdd_v8:
	!#PROLOGUE# 0
	save %sp,-112,%sp
	srl %i3,4,%o3
	and %i3,15,%o1
	sll %i1,2,%o0
	add %i4,%o0,%l2
	mov 16,%o0
	sub %o0,%o1,%o0
	sll %o0,2,%o0
	sub %i0,%o0,%i0
	sub %i2,%o0,%i2
	sub %i4,%o0,%i4
	addcc  %g0,%g0,%o2
	add %o1,-1,%o1
	cmp %o1,14
	bgu .LL3
	sll %o1,2,%o1
	
1:
	call 2f
	add %o7,%lo(.LL19-1b),%o0
2:
	ld [%o0+%o1],%o0
	jmp %o7+%o0
	addcc  %g0,%g0,%o2
	.align 4
.LL19:
	.word	.LL18-1b
	.word	.LL17-1b
	.word	.LL16-1b
	.word	.LL15-1b
	.word	.LL14-1b
	.word	.LL13-1b
	.word	.LL12-1b
	.word	.LL11-1b
	.word	.LL10-1b
	.word	.LL9-1b
	.word	.LL8-1b
	.word	.LL7-1b
	.word	.LL6-1b
	.word	.LL5-1b
	.word	.LL4-1b
.LL4:
	ld      [%i0+4],%o0
	ld      [%i2+4],%o2
	addxcc  %o2,%o0,%o0
	st      %o0,[%i4+4]
.LL5:
	ld      [%i0+8],%o0
	ld      [%i2+8],%o2
	addxcc  %o2,%o0,%o0
	st      %o0,[%i4+8]
.LL6:
	ld      [%i0+12],%o0
	ld      [%i2+12],%o2
	addxcc  %o2,%o0,%o0
	st      %o0,[%i4+12]
.LL7:
	ld      [%i0+16],%o0
	ld      [%i2+16],%o2
	addxcc  %o2,%o0,%o0
	st      %o0,[%i4+16]
.LL8:
	ld      [%i0+20],%o0
	ld      [%i2+20],%o2
	addxcc  %o2,%o0,%o0
	st      %o0,[%i4+20]
.LL9:
	ld      [%i0+24],%o0
	ld      [%i2+24],%o2
	addxcc  %o2,%o0,%o0
	st      %o0,[%i4+24]
.LL10:
	ld      [%i0+28],%o0
	ld      [%i2+28],%o2
	addxcc  %o2,%o0,%o0
	st      %o0,[%i4+28]
.LL11:
	ld      [%i0+32],%o0
	ld      [%i2+32],%o2
	addxcc  %o2,%o0,%o0
	st      %o0,[%i4+32]
.LL12:
	ld      [%i0+36],%o0
	ld      [%i2+36],%o2
	addxcc  %o2,%o0,%o0
	st      %o0,[%i4+36]
.LL13:
	ld      [%i0+40],%o0
	ld      [%i2+40],%o2
	addxcc  %o2,%o0,%o0
	st      %o0,[%i4+40]
.LL14:
	ld      [%i0+44],%o0
	ld      [%i2+44],%o2
	addxcc  %o2,%o0,%o0
	st      %o0,[%i4+44]
.LL15:
	ld      [%i0+48],%o0
	ld      [%i2+48],%o2
	addxcc  %o2,%o0,%o0
	st      %o0,[%i4+48]
.LL16:
	ld      [%i0+52],%o0
	ld      [%i2+52],%o2
	addxcc  %o2,%o0,%o0
	st      %o0,[%i4+52]
.LL17:
	ld      [%i0+56],%o0
	ld      [%i2+56],%o2
	addxcc  %o2,%o0,%o0
	st      %o0,[%i4+56]
.LL18:
	ld      [%i0+60],%o0
	ld      [%i2+60],%o2
	addxcc  %o2,%o0,%o0
	st      %o0,[%i4+60]
.LL3:
	addx  %g0,%g0,%l1
	b .LL34
	add %i0,64,%i0
.LL23:
	addcc -1,%l1,%l1
	ld      [%i0+0],%o0
	ld      [%i2+0],%o2
	addxcc  %o2,%o0,%o0
	st      %o0,[%i4+0]
	ld      [%i0+4],%o0
	ld      [%i2+4],%o2
	addxcc  %o2,%o0,%o0
	st      %o0,[%i4+4]
	ld      [%i0+8],%o0
	ld      [%i2+8],%o2
	addxcc  %o2,%o0,%o0
	st      %o0,[%i4+8]
	ld      [%i0+12],%o0
	ld      [%i2+12],%o2
	addxcc  %o2,%o0,%o0
	st      %o0,[%i4+12]
	ld      [%i0+16],%o0
	ld      [%i2+16],%o2
	addxcc  %o2,%o0,%o0
	st      %o0,[%i4+16]
	ld      [%i0+20],%o0
	ld      [%i2+20],%o2
	addxcc  %o2,%o0,%o0
	st      %o0,[%i4+20]
	ld      [%i0+24],%o0
	ld      [%i2+24],%o2
	addxcc  %o2,%o0,%o0
	st      %o0,[%i4+24]
	ld      [%i0+28],%o0
	ld      [%i2+28],%o2
	addxcc  %o2,%o0,%o0
	st      %o0,[%i4+28]
	ld      [%i0+32],%o0
	ld      [%i2+32],%o2
	addxcc  %o2,%o0,%o0
	st      %o0,[%i4+32]
	ld      [%i0+36],%o0
	ld      [%i2+36],%o2
	addxcc  %o2,%o0,%o0
	st      %o0,[%i4+36]
	ld      [%i0+40],%o0
	ld      [%i2+40],%o2
	addxcc  %o2,%o0,%o0
	st      %o0,[%i4+40]
	ld      [%i0+44],%o0
	ld      [%i2+44],%o2
	addxcc  %o2,%o0,%o0
	st      %o0,[%i4+44]
	ld      [%i0+48],%o0
	ld      [%i2+48],%o2
	addxcc  %o2,%o0,%o0
	st      %o0,[%i4+48]
	ld      [%i0+52],%o0
	ld      [%i2+52],%o2
	addxcc  %o2,%o0,%o0
	st      %o0,[%i4+52]
	ld      [%i0+56],%o0
	ld      [%i2+56],%o2
	addxcc  %o2,%o0,%o0
	st      %o0,[%i4+56]
	ld      [%i0+60],%o0
	ld      [%i2+60],%o2
	addxcc  %o2,%o0,%o0
	st      %o0,[%i4+60]
	addx  %g0,%g0,%l1
	add %i0,64,%i0
.LL34:
	add %i2,64,%i2
	add %o3,-1,%o3
	cmp %o3,-1
	bne .LL23
	add %i4,64,%i4
	cmp %i4,%i0
	be .LL25
	sub %i1,%i3,%l0
	sll %l0,2,%l0
	mov %i4,%o0
	mov %i0,%o1
	call memcpy,0
	mov %l0,%o2
	st %g0,[%i4+%l0]
.LL25:
	cmp %l1,0
	be .LL26
	nop
	b .LL35
	ld [%i4],%o0
.LL29:
	add %i4,4,%i4
	ld [%i4],%o0
.LL35:
	add %o0,1,%o0
	cmp %o0,0
	be .LL29
	st %o0,[%i4]
	cmp %i4,%l2
	be,a .LL26
	add %i1,1,%i1
.LL26:
	ret
	restore %g0,%i1,%o0
.LLfe2:
	.size	 _vecAdd_v8,.LLfe2-_vecAdd_v8
	.align 4
	.global _vecAddTo_v8
	.type	 _vecAddTo_v8,#function
	.proc	016
_vecAddTo_v8:
	!#PROLOGUE# 0
	save %sp,-112,%sp
	srl %i3,4,%o2
	and %i3,15,%i3
	sll %i1,2,%o0
	add %i0,%o0,%l0
	mov 16,%o0
	sub %o0,%i3,%o0
	sll %o0,2,%o0
	sub %i0,%o0,%i0
	sub %i2,%o0,%i2
	addcc  %g0,%g0,%o1
	add %i3,-1,%o1
	cmp %o1,14
	bgu .LL37
	sll %o1,2,%o1
	
1:
	call 2f
	add %o7,%lo(.LL53-1b),%o0
2:
	ld [%o0+%o1],%o0
	jmp %o7+%o0
	addcc  %g0,%g0,%o1
	.align 4
.LL53:
	.word	.LL52-1b
	.word	.LL51-1b
	.word	.LL50-1b
	.word	.LL49-1b
	.word	.LL48-1b
	.word	.LL47-1b
	.word	.LL46-1b
	.word	.LL45-1b
	.word	.LL44-1b
	.word	.LL43-1b
	.word	.LL42-1b
	.word	.LL41-1b
	.word	.LL40-1b
	.word	.LL39-1b
	.word	.LL38-1b
.LL38:
	ld      [%i0+4],%o0
	ld      [%i2+4],%o1
	addxcc  %o1,%o0,%o0
	st      %o0,[%i0+4]
.LL39:
	ld      [%i0+8],%o0
	ld      [%i2+8],%o1
	addxcc  %o1,%o0,%o0
	st      %o0,[%i0+8]
.LL40:
	ld      [%i0+12],%o0
	ld      [%i2+12],%o1
	addxcc  %o1,%o0,%o0
	st      %o0,[%i0+12]
.LL41:
	ld      [%i0+16],%o0
	ld      [%i2+16],%o1
	addxcc  %o1,%o0,%o0
	st      %o0,[%i0+16]
.LL42:
	ld      [%i0+20],%o0
	ld      [%i2+20],%o1
	addxcc  %o1,%o0,%o0
	st      %o0,[%i0+20]
.LL43:
	ld      [%i0+24],%o0
	ld      [%i2+24],%o1
	addxcc  %o1,%o0,%o0
	st      %o0,[%i0+24]
.LL44:
	ld      [%i0+28],%o0
	ld      [%i2+28],%o1
	addxcc  %o1,%o0,%o0
	st      %o0,[%i0+28]
.LL45:
	ld      [%i0+32],%o0
	ld      [%i2+32],%o1
	addxcc  %o1,%o0,%o0
	st      %o0,[%i0+32]
.LL46:
	ld      [%i0+36],%o0
	ld      [%i2+36],%o1
	addxcc  %o1,%o0,%o0
	st      %o0,[%i0+36]
.LL47:
	ld      [%i0+40],%o0
	ld      [%i2+40],%o1
	addxcc  %o1,%o0,%o0
	st      %o0,[%i0+40]
.LL48:
	ld      [%i0+44],%o0
	ld      [%i2+44],%o1
	addxcc  %o1,%o0,%o0
	st      %o0,[%i0+44]
.LL49:
	ld      [%i0+48],%o0
	ld      [%i2+48],%o1
	addxcc  %o1,%o0,%o0
	st      %o0,[%i0+48]
.LL50:
	ld      [%i0+52],%o0
	ld      [%i2+52],%o1
	addxcc  %o1,%o0,%o0
	st      %o0,[%i0+52]
.LL51:
	ld      [%i0+56],%o0
	ld      [%i2+56],%o1
	addxcc  %o1,%o0,%o0
	st      %o0,[%i0+56]
.LL52:
	ld      [%i0+60],%o0
	ld      [%i2+60],%o1
	addxcc  %o1,%o0,%o0
	st      %o0,[%i0+60]
.LL37:
	addx  %g0,%g0,%i3
	b .LL67
	add %i0,64,%i0
.LL57:
	addcc -1,%i3,%i3
	ld      [%i0+0],%o0
	ld      [%i2+0],%o1
	addxcc  %o1,%o0,%o0
	st      %o0,[%i0+0]
	ld      [%i0+4],%o0
	ld      [%i2+4],%o1
	addxcc  %o1,%o0,%o0
	st      %o0,[%i0+4]
	ld      [%i0+8],%o0
	ld      [%i2+8],%o1
	addxcc  %o1,%o0,%o0
	st      %o0,[%i0+8]
	ld      [%i0+12],%o0
	ld      [%i2+12],%o1
	addxcc  %o1,%o0,%o0
	st      %o0,[%i0+12]
	ld      [%i0+16],%o0
	ld      [%i2+16],%o1
	addxcc  %o1,%o0,%o0
	st      %o0,[%i0+16]
	ld      [%i0+20],%o0
	ld      [%i2+20],%o1
	addxcc  %o1,%o0,%o0
	st      %o0,[%i0+20]
	ld      [%i0+24],%o0
	ld      [%i2+24],%o1
	addxcc  %o1,%o0,%o0
	st      %o0,[%i0+24]
	ld      [%i0+28],%o0
	ld      [%i2+28],%o1
	addxcc  %o1,%o0,%o0
	st      %o0,[%i0+28]
	ld      [%i0+32],%o0
	ld      [%i2+32],%o1
	addxcc  %o1,%o0,%o0
	st      %o0,[%i0+32]
	ld      [%i0+36],%o0
	ld      [%i2+36],%o1
	addxcc  %o1,%o0,%o0
	st      %o0,[%i0+36]
	ld      [%i0+40],%o0
	ld      [%i2+40],%o1
	addxcc  %o1,%o0,%o0
	st      %o0,[%i0+40]
	ld      [%i0+44],%o0
	ld      [%i2+44],%o1
	addxcc  %o1,%o0,%o0
	st      %o0,[%i0+44]
	ld      [%i0+48],%o0
	ld      [%i2+48],%o1
	addxcc  %o1,%o0,%o0
	st      %o0,[%i0+48]
	ld      [%i0+52],%o0
	ld      [%i2+52],%o1
	addxcc  %o1,%o0,%o0
	st      %o0,[%i0+52]
	ld      [%i0+56],%o0
	ld      [%i2+56],%o1
	addxcc  %o1,%o0,%o0
	st      %o0,[%i0+56]
	ld      [%i0+60],%o0
	ld      [%i2+60],%o1
	addxcc  %o1,%o0,%o0
	st      %o0,[%i0+60]
	addx  %g0,%g0,%i3
	add %i0,64,%i0
.LL67:
	add %o2,-1,%o2
	cmp %o2,-1
	bne .LL57
	add %i2,64,%i2
	call do_nothing,0
	mov %i2,%o0
	cmp %i3,0
	be .LL59
	nop
	b .LL68
	ld [%i0],%o0
.LL62:
	add %i0,4,%i0
	ld [%i0],%o0
.LL68:
	add %o0,1,%o0
	cmp %o0,0
	be .LL62
	st %o0,[%i0]
	cmp %i0,%l0
	be,a .LL59
	add %i1,1,%i1
.LL59:
	ret
	restore %g0,%i1,%o0
.LLfe3:
	.size	 _vecAddTo_v8,.LLfe3-_vecAddTo_v8
	.align 4
	.global _vecSub_v8
	.type	 _vecSub_v8,#function
	.proc	016
_vecSub_v8:
	!#PROLOGUE# 0
	save %sp,-112,%sp
	srl %i3,4,%o3
	and %i3,15,%o1
	sll %i1,2,%o0
	add %i4,%o0,%l1
	mov 16,%o0
	sub %o0,%o1,%o0
	sll %o0,2,%o0
	sub %i0,%o0,%i0
	sub %i2,%o0,%i2
	sub %i4,%o0,%i4
	addcc  %g0,%g0,%o2
	add %o1,-1,%o1
	cmp %o1,14
	bgu .LL70
	sll %o1,2,%o1
	
1:
	call 2f
	add %o7,%lo(.LL86-1b),%o0
2:
	ld [%o0+%o1],%o0
	jmp %o7+%o0
	addcc  %g0,%g0,%o2
	.align 4
.LL86:
	.word	.LL85-1b
	.word	.LL84-1b
	.word	.LL83-1b
	.word	.LL82-1b
	.word	.LL81-1b
	.word	.LL80-1b
	.word	.LL79-1b
	.word	.LL78-1b
	.word	.LL77-1b
	.word	.LL76-1b
	.word	.LL75-1b
	.word	.LL74-1b
	.word	.LL73-1b
	.word	.LL72-1b
	.word	.LL71-1b
.LL71:
	ld      [%i0+4],%o0
	ld      [%i2+4],%o2
	subxcc  %o0,%o2,%o0
	st      %o0,[%i4+4]
.LL72:
	ld      [%i0+8],%o0
	ld      [%i2+8],%o2
	subxcc  %o0,%o2,%o0
	st      %o0,[%i4+8]
.LL73:
	ld      [%i0+12],%o0
	ld      [%i2+12],%o2
	subxcc  %o0,%o2,%o0
	st      %o0,[%i4+12]
.LL74:
	ld      [%i0+16],%o0
	ld      [%i2+16],%o2
	subxcc  %o0,%o2,%o0
	st      %o0,[%i4+16]
.LL75:
	ld      [%i0+20],%o0
	ld      [%i2+20],%o2
	subxcc  %o0,%o2,%o0
	st      %o0,[%i4+20]
.LL76:
	ld      [%i0+24],%o0
	ld      [%i2+24],%o2
	subxcc  %o0,%o2,%o0
	st      %o0,[%i4+24]
.LL77:
	ld      [%i0+28],%o0
	ld      [%i2+28],%o2
	subxcc  %o0,%o2,%o0
	st      %o0,[%i4+28]
.LL78:
	ld      [%i0+32],%o0
	ld      [%i2+32],%o2
	subxcc  %o0,%o2,%o0
	st      %o0,[%i4+32]
.LL79:
	ld      [%i0+36],%o0
	ld      [%i2+36],%o2
	subxcc  %o0,%o2,%o0
	st      %o0,[%i4+36]
.LL80:
	ld      [%i0+40],%o0
	ld      [%i2+40],%o2
	subxcc  %o0,%o2,%o0
	st      %o0,[%i4+40]
.LL81:
	ld      [%i0+44],%o0
	ld      [%i2+44],%o2
	subxcc  %o0,%o2,%o0
	st      %o0,[%i4+44]
.LL82:
	ld      [%i0+48],%o0
	ld      [%i2+48],%o2
	subxcc  %o0,%o2,%o0
	st      %o0,[%i4+48]
.LL83:
	ld      [%i0+52],%o0
	ld      [%i2+52],%o2
	subxcc  %o0,%o2,%o0
	st      %o0,[%i4+52]
.LL84:
	ld      [%i0+56],%o0
	ld      [%i2+56],%o2
	subxcc  %o0,%o2,%o0
	st      %o0,[%i4+56]
.LL85:
	ld      [%i0+60],%o0
	ld      [%i2+60],%o2
	subxcc  %o0,%o2,%o0
	st      %o0,[%i4+60]
.LL70:
	addx  %g0,%g0,%l0
	b .LL106
	add %i0,64,%i0
.LL90:
	addcc -1,%l0,%l0
	ld      [%i0+0],%o0
	ld      [%i2+0],%o2
	subxcc  %o0,%o2,%o0
	st      %o0,[%i4+0]
	ld      [%i0+4],%o0
	ld      [%i2+4],%o2
	subxcc  %o0,%o2,%o0
	st      %o0,[%i4+4]
	ld      [%i0+8],%o0
	ld      [%i2+8],%o2
	subxcc  %o0,%o2,%o0
	st      %o0,[%i4+8]
	ld      [%i0+12],%o0
	ld      [%i2+12],%o2
	subxcc  %o0,%o2,%o0
	st      %o0,[%i4+12]
	ld      [%i0+16],%o0
	ld      [%i2+16],%o2
	subxcc  %o0,%o2,%o0
	st      %o0,[%i4+16]
	ld      [%i0+20],%o0
	ld      [%i2+20],%o2
	subxcc  %o0,%o2,%o0
	st      %o0,[%i4+20]
	ld      [%i0+24],%o0
	ld      [%i2+24],%o2
	subxcc  %o0,%o2,%o0
	st      %o0,[%i4+24]
	ld      [%i0+28],%o0
	ld      [%i2+28],%o2
	subxcc  %o0,%o2,%o0
	st      %o0,[%i4+28]
	ld      [%i0+32],%o0
	ld      [%i2+32],%o2
	subxcc  %o0,%o2,%o0
	st      %o0,[%i4+32]
	ld      [%i0+36],%o0
	ld      [%i2+36],%o2
	subxcc  %o0,%o2,%o0
	st      %o0,[%i4+36]
	ld      [%i0+40],%o0
	ld      [%i2+40],%o2
	subxcc  %o0,%o2,%o0
	st      %o0,[%i4+40]
	ld      [%i0+44],%o0
	ld      [%i2+44],%o2
	subxcc  %o0,%o2,%o0
	st      %o0,[%i4+44]
	ld      [%i0+48],%o0
	ld      [%i2+48],%o2
	subxcc  %o0,%o2,%o0
	st      %o0,[%i4+48]
	ld      [%i0+52],%o0
	ld      [%i2+52],%o2
	subxcc  %o0,%o2,%o0
	st      %o0,[%i4+52]
	ld      [%i0+56],%o0
	ld      [%i2+56],%o2
	subxcc  %o0,%o2,%o0
	st      %o0,[%i4+56]
	ld      [%i0+60],%o0
	ld      [%i2+60],%o2
	subxcc  %o0,%o2,%o0
	st      %o0,[%i4+60]
	addx  %g0,%g0,%l0
	add %i0,64,%i0
.LL106:
	add %i2,64,%i2
	add %o3,-1,%o3
	cmp %o3,-1
	bne .LL90
	add %i4,64,%i4
	call do_nothing,0
	mov %i2,%o0
	cmp %i4,%i0
	be .LL92
	mov %i4,%o0
	sub %i1,%i3,%o2
	mov %i0,%o1
	call memcpy,0
	sll %o2,2,%o2
.LL92:
	cmp %l0,0
	be,a .LL108
	add %l1,-4,%l1
	b .LL107
	ld [%i4],%o0
.LL96:
	add %i4,4,%i4
	ld [%i4],%o0
.LL107:
	add %o0,-1,%o0
	cmp %o0,-1
	be .LL96
	st %o0,[%i4]
	b .LL108
	add %l1,-4,%l1
.LL100:
	add %l1,-4,%l1
.LL108:
	ld [%l1],%o0
	cmp %o0,0
	be,a .LL100
	add %i1,-1,%i1
	ret
	restore %g0,%i1,%o0
.LLfe4:
	.size	 _vecSub_v8,.LLfe4-_vecSub_v8
	.align 4
	.global _vecSubFrom_v8
	.type	 _vecSubFrom_v8,#function
	.proc	016
_vecSubFrom_v8:
	!#PROLOGUE# 0
	save %sp,-112,%sp
	srl %i3,4,%o2
	and %i3,15,%i3
	sll %i1,2,%o0
	add %i0,%o0,%l0
	mov 16,%o0
	sub %o0,%i3,%o0
	sll %o0,2,%o0
	sub %i0,%o0,%i0
	sub %i2,%o0,%i2
	addcc  %g0,%g0,%o1
	add %i3,-1,%o1
	cmp %o1,14
	bgu .LL110
	sll %o1,2,%o1
	
1:
	call 2f
	add %o7,%lo(.LL126-1b),%o0
2:
	ld [%o0+%o1],%o0
	jmp %o7+%o0
	addcc  %g0,%g0,%o1
	.align 4
.LL126:
	.word	.LL125-1b
	.word	.LL124-1b
	.word	.LL123-1b
	.word	.LL122-1b
	.word	.LL121-1b
	.word	.LL120-1b
	.word	.LL119-1b
	.word	.LL118-1b
	.word	.LL117-1b
	.word	.LL116-1b
	.word	.LL115-1b
	.word	.LL114-1b
	.word	.LL113-1b
	.word	.LL112-1b
	.word	.LL111-1b
.LL111:
	ld      [%i0+4],%o0
	ld      [%i2+4],%o1
	subxcc  %o0,%o1,%o0
	st      %o0,[%i0+4]
.LL112:
	ld      [%i0+8],%o0
	ld      [%i2+8],%o1
	subxcc  %o0,%o1,%o0
	st      %o0,[%i0+8]
.LL113:
	ld      [%i0+12],%o0
	ld      [%i2+12],%o1
	subxcc  %o0,%o1,%o0
	st      %o0,[%i0+12]
.LL114:
	ld      [%i0+16],%o0
	ld      [%i2+16],%o1
	subxcc  %o0,%o1,%o0
	st      %o0,[%i0+16]
.LL115:
	ld      [%i0+20],%o0
	ld      [%i2+20],%o1
	subxcc  %o0,%o1,%o0
	st      %o0,[%i0+20]
.LL116:
	ld      [%i0+24],%o0
	ld      [%i2+24],%o1
	subxcc  %o0,%o1,%o0
	st      %o0,[%i0+24]
.LL117:
	ld      [%i0+28],%o0
	ld      [%i2+28],%o1
	subxcc  %o0,%o1,%o0
	st      %o0,[%i0+28]
.LL118:
	ld      [%i0+32],%o0
	ld      [%i2+32],%o1
	subxcc  %o0,%o1,%o0
	st      %o0,[%i0+32]
.LL119:
	ld      [%i0+36],%o0
	ld      [%i2+36],%o1
	subxcc  %o0,%o1,%o0
	st      %o0,[%i0+36]
.LL120:
	ld      [%i0+40],%o0
	ld      [%i2+40],%o1
	subxcc  %o0,%o1,%o0
	st      %o0,[%i0+40]
.LL121:
	ld      [%i0+44],%o0
	ld      [%i2+44],%o1
	subxcc  %o0,%o1,%o0
	st      %o0,[%i0+44]
.LL122:
	ld      [%i0+48],%o0
	ld      [%i2+48],%o1
	subxcc  %o0,%o1,%o0
	st      %o0,[%i0+48]
.LL123:
	ld      [%i0+52],%o0
	ld      [%i2+52],%o1
	subxcc  %o0,%o1,%o0
	st      %o0,[%i0+52]
.LL124:
	ld      [%i0+56],%o0
	ld      [%i2+56],%o1
	subxcc  %o0,%o1,%o0
	st      %o0,[%i0+56]
.LL125:
	ld      [%i0+60],%o0
	ld      [%i2+60],%o1
	subxcc  %o0,%o1,%o0
	st      %o0,[%i0+60]
.LL110:
	addx  %g0,%g0,%i3
	b .LL145
	add %i0,64,%i0
.LL130:
	addcc -1,%i3,%i3
	ld      [%i0+0],%o0
	ld      [%i2+0],%o1
	subxcc  %o0,%o1,%o0
	st      %o0,[%i0+0]
	ld      [%i0+4],%o0
	ld      [%i2+4],%o1
	subxcc  %o0,%o1,%o0
	st      %o0,[%i0+4]
	ld      [%i0+8],%o0
	ld      [%i2+8],%o1
	subxcc  %o0,%o1,%o0
	st      %o0,[%i0+8]
	ld      [%i0+12],%o0
	ld      [%i2+12],%o1
	subxcc  %o0,%o1,%o0
	st      %o0,[%i0+12]
	ld      [%i0+16],%o0
	ld      [%i2+16],%o1
	subxcc  %o0,%o1,%o0
	st      %o0,[%i0+16]
	ld      [%i0+20],%o0
	ld      [%i2+20],%o1
	subxcc  %o0,%o1,%o0
	st      %o0,[%i0+20]
	ld      [%i0+24],%o0
	ld      [%i2+24],%o1
	subxcc  %o0,%o1,%o0
	st      %o0,[%i0+24]
	ld      [%i0+28],%o0
	ld      [%i2+28],%o1
	subxcc  %o0,%o1,%o0
	st      %o0,[%i0+28]
	ld      [%i0+32],%o0
	ld      [%i2+32],%o1
	subxcc  %o0,%o1,%o0
	st      %o0,[%i0+32]
	ld      [%i0+36],%o0
	ld      [%i2+36],%o1
	subxcc  %o0,%o1,%o0
	st      %o0,[%i0+36]
	ld      [%i0+40],%o0
	ld      [%i2+40],%o1
	subxcc  %o0,%o1,%o0
	st      %o0,[%i0+40]
	ld      [%i0+44],%o0
	ld      [%i2+44],%o1
	subxcc  %o0,%o1,%o0
	st      %o0,[%i0+44]
	ld      [%i0+48],%o0
	ld      [%i2+48],%o1
	subxcc  %o0,%o1,%o0
	st      %o0,[%i0+48]
	ld      [%i0+52],%o0
	ld      [%i2+52],%o1
	subxcc  %o0,%o1,%o0
	st      %o0,[%i0+52]
	ld      [%i0+56],%o0
	ld      [%i2+56],%o1
	subxcc  %o0,%o1,%o0
	st      %o0,[%i0+56]
	ld      [%i0+60],%o0
	ld      [%i2+60],%o1
	subxcc  %o0,%o1,%o0
	st      %o0,[%i0+60]
	addx  %g0,%g0,%i3
	add %i0,64,%i0
.LL145:
	add %o2,-1,%o2
	cmp %o2,-1
	bne .LL130
	add %i2,64,%i2
	call do_nothing,0
	mov %i2,%o0
	cmp %i3,0
	be,a .LL147
	add %l0,-4,%l0
	b .LL146
	ld [%i0],%o0
.LL135:
	add %i0,4,%i0
	ld [%i0],%o0
.LL146:
	add %o0,-1,%o0
	cmp %o0,-1
	be .LL135
	st %o0,[%i0]
	b .LL147
	add %l0,-4,%l0
.LL139:
	add %l0,-4,%l0
.LL147:
	ld [%l0],%o0
	cmp %o0,0
	be,a .LL139
	add %i1,-1,%i1
	ret
	restore %g0,%i1,%o0
.LLfe5:
	.size	 _vecSubFrom_v8,.LLfe5-_vecSubFrom_v8
	.ident	"GCC: (GNU) 2.8.1"
