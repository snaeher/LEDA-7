.register %g2, #scratch
.register %g3, #scratch

	.globl	vecSubCarry 
	.type	vecSubCarry,#function
vecSubCarry:
	 save	%sp, -112, %sp
         mov %i0,%o0
         mov %i1,%o1
         mov %i2,%o2
         mov %i3,%o3
         call vecSubCarryC
         mov 0,%o4
         ret
 	 restore %g0, %o0, %o0

	.globl	vecSubCarryC 
	.type	vecSubCarryC,#function
vecSubCarryC:

        ! Input
        ! %o0: diff 
        ! %o1: a
        ! %o2: b
        ! %o3: b_used
        ! %o4: carry
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
 	sub	%g1,%o4,%g1
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
	ld  	[%o1],%g4
	sub	%g1,%o4,%g1
	add	%o2,8,%o2
	ld  	[%o2],%g5
	srlx	%g1,63,%o4
 	deccc	%o3
 	bz   	.VSC_Lend2
	st 	%g1,[%o0]

	sub	%g2,%g3,%g1
 	add	%o0,4,%o0
	ld  	[%o1+4],%g2
	sub	%g1,%o4,%g1
 	add	%o0,4,%o0
	ld  	[%o2+4],%g3
	srlx	%g1,63,%o4
  	deccc	%o3
 	bnz   	.VSC_Loop
	st 	%g1,[%o0-4]

.VSC_Lend1:
	sub	%g4,%g5,%g1
	sub	%g1,%o4,%g1
	st 	%g1,[%o0]
	srlx	%g1,63,%o4
	sub	%g2,%g3,%g1
	sub	%g1,%o4,%g1
	st 	%g1,[%o0+4]
	retl
	srlx	%g1,63,%o0

.VSC_Lend2:
	sub	%g2,%g3,%g1
	sub	%g1,%o4,%g1
	st 	%g1,[%o0+4]
	srlx	%g1,63,%o4
	sub	%g4,%g5,%g1
	sub	%g1,%o4,%g1
	st 	%g1,[%o0+8]
	retl
	srlx	%g1,63,%o0


