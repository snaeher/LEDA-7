!------------------------------------------------------------------------------
! extern "C" word digMulword a, word b, word* high);
! (returns lower 32 bit of product, stores higher 32 bit in *high)
!------------------------------------------------------------------------------

.seg "text"

.global _digMul_v8
_digMul_v8:	
umul %o0,%o1,%o0
rd   %y,%g1
retl
st   %g1,[%o2]

	
!------------------------------------------------------------------------------
! extern "C" word vecMulLoopFirst(word *p, word *a, word* a_stop, word B)
!------------------------------------------------------------------------------
! %o0 p
! %o1 a
! %o2 a_stop
! %o3 B
!
.global    _vecMulLoopFirst_v8
_vecMulLoopFirst_v8:

mov        0, %g1         ! carry = 0
FMULT_L0:                 ! Loop: while (a < a_stop)
cmp        %o1,%o2        ! if a >= a_stop return 
bge        FMULT_L1

ld         [%o1], %g2     ! A = *a
umul       %g2, %o3, %g2  ! %g2 = low of A*B
rd         %y, %g3        ! %g3 = high
addcc      %g2, %g1, %g2  ! low += carry
addx       %g0, %g3, %g1  ! new carry = high + carry from addition
st         %g2, [%o0]     ! *p = low
inc        4,   %o1       ! a++
b          FMULT_L0       ! loop
inc        4,   %o0       ! p++

FMULT_L1:
st         %g1, [%o0]     ! *p = carry
retl
mov        %g1, %o0       ! return carry



!------------------------------------------------------------------------------
! extern "C" word vecMulLoop(word *p, word *a, word* a_stop, word B)
!------------------------------------------------------------------------------
! %o0 p
! %o1 a
! %o2 a_stop
! %o3 B
!
.global    _vecMulLoop_v8
_vecMulLoop_v8:

mov        0, %g5         ! carry = 0

IMULT_L0:

add        %o1, 16, %o1
cmp        %o1,%o2        ! if (a+4) >= a_stop goto IMULT_L1
bge        IMULT_L1
add        %o1, -16, %o1

ld         [%o1+0], %g2   ! A = *a
ld         [%o0+0], %g4   ! P = *p
umul       %g2, %o3, %g2  ! %g2=low of *a*B
rd         %y, %g3        ! %g3=high
addcc      %g5, %g4, %g4  ! P += carry
addx       %g0, %g0, %g5  ! new carry from addition
addcc      %g2, %g4, %g2  ! low += P
addx       %g3, %g5, %g5  ! carry += high
st         %g2, [%o0+0]   ! *p = low

ld         [%o1+4], %g2     
ld         [%o0+4], %g4     
umul       %g2, %o3, %g2  
rd         %y, %g3       
addcc      %g5, %g4, %g4 
addx       %g0, %g0, %g5 
addcc      %g2, %g4, %g2 
addx       %g3, %g5, %g5 
st         %g2, [%o0+4]   

ld         [%o1+8], %g2     
ld         [%o0+8], %g4     
umul       %g2, %o3, %g2  
rd         %y, %g3       
addcc      %g5, %g4, %g4 
addx       %g0, %g0, %g5 
addcc      %g2, %g4, %g2 
addx       %g3, %g5, %g5 
st         %g2, [%o0+8]   

ld         [%o1+12], %g2     
ld         [%o0+12], %g4     
umul       %g2, %o3, %g2  
rd         %y, %g3       
addcc      %g5, %g4, %g4 
addx       %g0, %g0, %g5 
addcc      %g2, %g4, %g2 
addx       %g3, %g5, %g5 
st         %g2, [%o0+12]   

add        %o0,16,%o0
add        %o1,16,%o1
b          IMULT_L0
nop

IMULT_L1:
cmp        %o1,%o2        ! if a >= a_stop return 
bge        IMULT_L2
nop
ld         [%o1], %g2     ! A = *a
ld         [%o0], %g4     ! P = *p
umul       %g2, %o3, %g2  ! %g2=low of *a*B
rd         %y, %g3        ! %g3=high
addcc      %g5, %g4, %g4  ! P += carry
addx       %g0, %g0, %g5  ! new carry from addition
addcc      %g2, %g4, %g2  ! low += P
addx       %g3, %g5, %g5  ! carry += high
st         %g2, [%o0]     ! *p = low
inc        4,   %o1       ! a++
b          IMULT_L1       ! loop
inc        4,   %o0       ! p++
IMULT_L2:
st         %g5, [%o0]     ! *p = carry
retl
mov        %g5, %o0       ! return carry


