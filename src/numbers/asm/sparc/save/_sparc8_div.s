
!------------------------------------------------------------------------------
! assembler code for unsigned 32 bit multiplication
! taken from 
!
!            "The SPARC Architecture Manual"  (Appendix E.2)
!
!------------------------------------------------------------------------------


!------------------------------------------------------------------------------
! extern "C" word Div_Inner_Loop_v8(word *p, word *a, word* a_stop, word B)
!------------------------------------------------------------------------------
! %o0 p
! %o1 a
! %o2 a_stop
! %o3 B
!

.global    _vecDivLoop_v8
_vecDivLoop_v8:
cmp        %o1,%o2       ! if a >= a_stop return 
bge        IDIV_L1
mov        0, %g5        ! carry = 0
IDIV_L0:                 ! Loop: while (a < a_stop)

ld         [%o1], %g2    ! A = *a
ld         [%o0], %g4    ! P = *p

umul %g2,%o3,%g2 ! %g2=low of *a*B
rd %y,%g3        ! %g3=high

subcc      %g4, %g5, %g4  ! P -= carry
addx       %g0, %g0, %g5  ! new carry from subtraction
subcc      %g4, %g2, %g4  ! P -= low
addx       %g3, %g5, %g5  ! carry += high
st         %g4, [%o0]     ! *p = P
inc        4,   %o1       ! a++
cmp        %o1, %o2       ! if (a < a_stop) goto IDIV_L0
bl         IDIV_L0        ! loop
inc        4,   %o0       ! p++
IDIV_L1:
ld         [%o0], %g4     ! P = *p
subcc      %g4, %g5, %g4  ! P -= carry
addx       %g0, %g0, %g5  ! new carry 
retl
mov        %g5, %o0       ! return carry



!------------------------------------------------------------------------------
! extern "C" word DivTwoDigitsByOne(word a1, word a2, word b, word *q);
! (returns the remainder of [a1,a2]=q*b+rem
!------------------------------------------------------------------------------

.seg "text"

.global _digDivTwoByOne_v8
_digDivTwoByOne_v8:	
! arguments are given in %o0,%o1,%o2,%o3
! 
! output rem,q = %o0,[%o3]

wr   %o0,%y	
udiv %o1,%o2,%o4   ! divide [y,o1] by o2=b . q goes to o4
st   %o4,[%o3]     ! set *q, is nowcorrect
umul %o4,%o2,%o4   ! backmultiplication q*b. low part goes to o4
sub  %o1,%o4,%o0   ! [a1,a2]-q*b, but only low part, goes to o0		
retl               ! leaf routine return
nop

	
