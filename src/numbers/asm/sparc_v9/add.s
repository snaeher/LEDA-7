.register %g2, #scratch
.register %g3, #scratch

	.globl	vecAddCarry 
	.type	vecAddCarry,#function
vecAddCarry:
	 save	%sp, -112, %sp
         mov %i0,%o0
         mov %i1,%o1
         mov %i2,%o2
         mov %i3,%o3
         call vecAddCarryC
         mov 0,%o4
         ret
 	 restore %g0, %o0, %o0

	.globl	vecAddCarryC 
	.type	vecAddCarryC,#function
vecAddCarryC:

        ! Input
        ! %o0: sum 
        ! %o1: a
        ! %o2: b
        ! %o3: b_used
        ! %o4: carry
        !
        ! Local
        ! %g1  sum/carry
        ! %g2  a[2i+1]
        ! %g3  b[2i+1]
        ! %g4  a[2i]
        ! %g5  b[2i]



	ld  	[%o1],%g4
	ld  	[%o2],%g5
 	deccc	%o3
	bnz   	.VAC_L1
	add	%g4,%g5,%g1
 	add	%g1,%o4,%g1
	st 	%g1,[%o0]
	retl
	srlx	%g1,32,%o0


.VAC_L1:
	ld  	[%o1+4],%g2
	ld  	[%o2+4],%g3
 	deccc	%o3
	bz   	.VAC_Lend1
        nop

.VAC_Loop:
	add	%g4,%g5,%g1
	add	%o1,8,%o1
	ld  	[%o1],%g4
	add	%g1,%o4,%g1
	add	%o2,8,%o2
	ld  	[%o2],%g5
	srlx	%g1,32,%o4
 	deccc	%o3
 	bz   	.VAC_Lend2
	st 	%g1,[%o0]

	add	%g2,%g3,%g1
 	add	%o0,4,%o0
	ld  	[%o1+4],%g2
	add	%g1,%o4,%g1
 	add	%o0,4,%o0
	ld  	[%o2+4],%g3
	srlx	%g1,32,%o4
  	deccc	%o3
 	bnz   	.VAC_Loop
	st 	%g1,[%o0-4]

.VAC_Lend1:
	add	%g4,%g5,%g1
	add	%g1,%o4,%g1
	st 	%g1,[%o0]
	srlx	%g1,32,%o4
	add	%g2,%g3,%g1
	add	%g1,%o4,%g1
	st 	%g1,[%o0+4]
	retl
	srlx	%g1,32,%o0

.VAC_Lend2:
	add	%g2,%g3,%g1
	add	%g1,%o4,%g1
	st 	%g1,[%o0+4]
	srlx	%g1,32,%o4
	add	%g4,%g5,%g1
	add	%g1,%o4,%g1
	st 	%g1,[%o0+8]
	retl
	srlx	%g1,32,%o0




	.globl	vecAddCarryC16
	.type	vecAddCarryC16,#function
vecAddCarryC16:

        ! Input
        ! %o0: sum 
        ! %o1: a
        ! %o2: b
        ! %o3: carry
        !
        ! Local
        ! %g1  a[2i]
        ! %g2  b[2i]
        ! %g3  a[2i+1]
        ! %g4  b[2i+1]

	ld  	[%o1],%g1
	ld  	[%o2],%g2

        add     %o3,%g2,%g2 ! carry
	ld  	[%o1+4],%g3
	add	%g1,%g2,%g2
	ld  	[%o2+4],%g4
	st 	%g2,[%o0]
	srlx	%g2,32,%o3

        add     %o3,%g4,%g4
	ld  	[%o1+8],%g1
	add	%g3,%g4,%g4
	ld  	[%o2+8],%g2
	st 	%g4,[%o0+4]
	srlx	%g4,32,%o3

        add     %o3,%g2,%g2
	ld  	[%o1+12],%g3
	add	%g1,%g2,%g2
	ld  	[%o2+12],%g4
	st 	%g2,[%o0+8]
	srlx	%g2,32,%o3

        add     %o3,%g4,%g4
	ld  	[%o1+16],%g1
	add	%g3,%g4,%g4
	ld  	[%o2+16],%g2
	st 	%g4,[%o0+12]
	srlx	%g4,32,%o3

        add     %o3,%g2,%g2
	ld  	[%o1+20],%g3
	add	%g1,%g2,%g2
	ld  	[%o2+20],%g4
	st 	%g2,[%o0+16]
	srlx	%g2,32,%o3

        add     %o3,%g4,%g4
	ld  	[%o1+24],%g1
	add	%g3,%g4,%g4
	ld  	[%o2+24],%g2
	st 	%g4,[%o0+20]
	srlx	%g4,32,%o3

        add     %o3,%g2,%g2
	ld  	[%o1+28],%g3
	add	%g1,%g2,%g2
	ld  	[%o2+28],%g4
	st 	%g2,[%o0+24]
	srlx	%g2,32,%o3

        add     %o3,%g4,%g4
	ld  	[%o1+32],%g1
	add	%g3,%g4,%g4
	ld  	[%o2+32],%g2
	st 	%g4,[%o0+28]
	srlx	%g4,32,%o3

        add     %o3,%g2,%g2
	ld  	[%o1+36],%g3
	add	%g1,%g2,%g2
	ld  	[%o2+36],%g4
	st 	%g2,[%o0+32]
	srlx	%g2,32,%o3

        add     %o3,%g4,%g4
	ld  	[%o1+40],%g1
	add	%g3,%g4,%g4
	ld  	[%o2+40],%g2
	st 	%g4,[%o0+36]
	srlx	%g4,32,%o3

        add     %o3,%g2,%g2
	ld  	[%o1+44],%g3
	add	%g1,%g2,%g2
	ld  	[%o2+44],%g4
	st 	%g2,[%o0+40]
	srlx	%g2,32,%o3

        add     %o3,%g4,%g4
	ld  	[%o1+48],%g1
	add	%g3,%g4,%g4
	ld  	[%o2+48],%g2
	st 	%g4,[%o0+44]
	srlx	%g4,32,%o3

        add     %o3,%g2,%g2
	ld  	[%o1+52],%g3
	add	%g1,%g2,%g2
	ld  	[%o2+52],%g4
	st 	%g2,[%o0+48]
	srlx	%g2,32,%o3

        add     %o3,%g4,%g4
	ld  	[%o1+56],%g1
	add	%g3,%g4,%g4
	ld  	[%o2+56],%g2
	st 	%g4,[%o0+52]
	srlx	%g4,32,%o3

        add     %o3,%g2,%g2
	ld  	[%o1+60],%g3
	add	%g1,%g2,%g2
	ld  	[%o2+60],%g4
	st 	%g2,[%o0+56]
	srlx	%g2,32,%o3


        add     %o3,%g4,%g4
	add	%g3,%g4,%g4
	st 	%g4,[%o0+60]
	retl
	srlx	%g4,32,%o0


