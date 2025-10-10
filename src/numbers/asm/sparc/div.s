.seg "text"

	
!------------------------------------------------------------------------------
! extern "C" digit vecDivInnerLoop(digit *p, digit *a, digit* a_stop, digit B)
!------------------------------------------------------------------------------
! %o0 p
! %o1 a
! %o2 a_stop
! %o3 B
!

.global    vecDivInnerLoop
vecDivInnerLoop:

mov        0, %g4         ! carry = 0

IDIV_L0:

ld         [%o0], %g1     ! P = *p
ld         [%o1], %g2     ! A = *a

/*
mulx       %g2, %o3, %g2 
add        %g2, %g4, %g2  ! g2 += carry
srlx       %g2, 32, %g4   ! new carry
subcc      %g1, %g2, %g1  ! P -= g2
addx       %g4, 0, %g4    ! carry ?
st         %g1, [%o0]     ! *p = P
*/

umul       %g2,%o3,%g2    ! %g2=low of *a*B
rd         %y,%g3         ! %g3=high
addcc      %g2, %g4, %g2  ! g2 += carry
addx       %g3, 0, %g4    ! carry ?
subcc      %g1, %g2, %g1  ! P -= g2
addx       %g4, 0, %g4    ! carry ?
st         %g1, [%o0]     ! *p = P

inc        4,   %o1       ! a++
cmp        %o1, %o2       ! if (a < a_stop) goto IDIV_L0
bl         IDIV_L0        ! loop
inc        4,   %o0       ! p++


ld         [%o0], %g1     ! P = *p
subcc      %g1, %g4, %g1  ! P -= carry
st         %g1, [%o0]    
retl
addx       %g0, %g0, %o0  ! new carry 



/*
.global    vecDivInnerLoop
vecDivInnerLoop:

! %o0 p
! %o1 a
! %o2 a_stop
! %o3 B


mov    0xffffffff, %g3    ! g3: 0xffffffff  (- borrow)
sllx   %g3, 32,  %g6
sub    %g6, %g3, %g6      ! g6: 0xfffffffe00000001


IDIV_L0:

ld         [%o1], %g1    ! src
ld         [%o0], %g2    ! dest

add        %g6, %g2, %g2
mulx       %o3, %g1, %g1 
sub        %g2, %g1, %g2
add        %g2, %g3, %g3
st         %g3, [%o0]
srlx       %g3, 32, %g3

inc        4,   %o1
cmp        %o1, %o2
bl         IDIV_L0
inc        4,   %o0

ld         [%o0], %g2
add        %g6, %g2, %g2
add        %g2, %g3, %g3
st         %g3, [%o0]
srlx       %g3, 32, %g3
mov        0xffffffff,%o0

retl
sub        %o0,%g3,%o0

*/





!------------------------------------------------------------------------------
! extern "C" digit DivTwoDigitsByOne(digit a1, digit a2, digit b, digit *q);
! (returns the remainder of [a1,a2]=q*b+rem
!------------------------------------------------------------------------------

.seg "text"

.global _digDivTwoByOne
_digDivTwoByOne:	
! output rem,q = %o0,[%o3]

wr   %o0,%y	
udiv %o1,%o2,%o4   ! divide [y,o1] by o2=b . q goes to o4
st   %o4,[%o3]     ! set *q, is nowcorrect
umul %o4,%o2,%o4   ! backmultiplication q*b. low part goes to o4
sub  %o1,%o4,%o0   ! [a1,a2]-q*b, but only low part, goes to o0		
retl               ! leaf routine return
nop

	
