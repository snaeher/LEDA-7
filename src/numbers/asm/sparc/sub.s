

	.globl	vecSubCarry 
	.type	vecSubCarry,#function
vecSubCarry:
        ! Input
        ! %o0: diff 
        ! %o1: a
        ! %o2: b
        ! %o3: b_used
        !
        ! Local
        ! %g1  diff/carry
        ! %g2  a[2i+1]
        ! %g3  b[2i+1]
        ! %g4  a[2i]
        ! %g5  b[2i]

	ld  	[%o1],%g4
	ld  	[%o2],%g5
 	deccc	%o3
	bnz   	.VSC_L1
	sub	%g4,%g5,%g1
	st 	%g1,[%o0]
	retl
	srlx	%g1,63,%o0


.VSC_L1:
	ld  	[%o1+4],%g2
	ld  	[%o2+4],%g3
 	deccc	%o3
	bz   	.VSC_Lend1
        nop

.VSC_Loop:
	sub	%g4,%g5,%g1
	add	%o1,8,%o1
	add	%o2,8,%o2
	st 	%g1,[%o0]
 	add	%o0,4,%o0
	srlx	%g1,63,%g1
	ld  	[%o1],%g4
 	add     %g3,%g1,%g3
 	deccc	%o3
 	bz   	.VSC_Lend2
	ld  	[%o2],%g5

	sub	%g2,%g3,%g1
	st 	%g1,[%o0]
 	add	%o0,4,%o0
	srlx	%g1,63,%g1
	ld  	[%o1+4],%g2
 	add     %g5,%g1,%g5
  	deccc	%o3
 	bnz   	.VSC_Loop
	ld  	[%o2+4],%g3

.VSC_Lend1:
	sub	%g4,%g5,%g1
	st 	%g1,[%o0]
	srlx	%g1,63,%g1
 	add     %g3,%g1,%g3
	sub	%g2,%g3,%g1
	st 	%g1,[%o0+4]
	retl
	srlx	%g1,63,%o0

.VSC_Lend2:
	sub	%g2,%g3,%g1
	st 	%g1,[%o0]
	srlx	%g1,63,%g1
 	add     %g5,%g1,%g5
	sub	%g4,%g5,%g1
	st 	%g1,[%o0+4]
	retl
	srlx	%g1,63,%o0

