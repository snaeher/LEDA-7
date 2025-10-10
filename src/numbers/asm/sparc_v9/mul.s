.register %g2, #scratch
.register %g3, #scratch

!------------------------------------------------------------------------------
! extern "C" digit digMul(digit a, digit b, digit* high);
! (returns lower 32 bit of product, stores higher 32 bit in *high)
!------------------------------------------------------------------------------

.seg "text"

.global digMul
digMul:	
mulx %o0,%o1,%o0
srlx %o0,32,%g1
retl
st   %g1,[%o2]


!------------------------------------------------------------------------------
! extern "C" digit vecMulInnerLoop(digit *p, digit *a, digit* a_stop, digit B)
!------------------------------------------------------------------------------
! %o0 p
! %o1 a
! %o2 a_stop
!
! %g1  carry

.global    vecMulInnerLoop
vecMulInnerLoop:

mov        0, %g1

vMIL_L0:                    
	
ld         [%o1], %g2  
ld         [%o0], %g3
mulx       %o3, %g2, %g2
add        %g3, %g1, %g1
add        %g1, %g2, %g2
inc        4, %o1
st         %g2, [%o0]
srlx       %g2, 32, %g1

cmp        %o1, %o2
bl         vMIL_L0
inc        4, %o0

st         %g1, [%o0]
retl
mov        %g1, %o0



!------------------------------------------------------------------------------
! extern "C" digit vecMulLoopFirst(digit *p, digit *a, digit* a_stop, digit B)
!------------------------------------------------------------------------------
! %o0 p
! %o1 a
! %o2 a_stop
!
! %g1  carry

.global    vecMulLoopFirst
vecMulLoopFirst:

mov        0, %g1

vMLF_L0:                    
	
ld         [%o1], %g2  
inc        4, %o1
mulx       %o3, %g2, %g2
add        %g1, %g2, %g2
st         %g2, [%o0]
srlx       %g2, 32, %g1

cmp        %o1, %o2
bl         vMLF_L0
inc        4, %o0

st         %g1, [%o0]
retl
mov        %g1, %o0



