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

vMLF_Loop:                    
	
ld         [%o1], %g2  
inc        4, %o1
mulx       %o3, %g2, %g2
add        %g1, %g2, %g2
st         %g2, [%o0]
srlx       %g2, 32, %g1

cmp        %o1, %o2
bl         vMLF_Loop
inc        4, %o0

st         %g1, [%o0]
retl
mov        %g1, %o0



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

vMIL_Loop:                    
	
ld         [%o1], %g2  
ld         [%o0], %g3
mulx       %o3, %g2, %g2
add        %g3, %g1, %g1
add        %g1, %g2, %g2
inc        4, %o1
st         %g2, [%o0]
srlx       %g2, 32, %g1

cmp        %o1, %o2
bl         vMIL_Loop
inc        4, %o0

st         %g1, [%o0]
retl
mov        %g1, %o0


!------------------------------------------------------------------------------
! extern "C" digit vecSqrInnerLoop(digit *p, digit *a, digit* a_stop)
!------------------------------------------------------------------------------
! %o0 p
! %o1 a
! %o2 a_stop
!
! %g1  carry



.global    vecSqrInnerLoop
vecSqrInnerLoop:

mov        0, %g1

vSIL_Loop:                    
	
ld         [%o1], %g2  
ld         [%o0], %g3
mulx       %g2, %g2, %g2
add        %g3, %g1, %g1
add        %g2, %g1, %g1
st         %g1, [%o0]
srlx       %g1, 32, %g1

ld         [4+%o0], %g3
add        %g3, %g1, %g1
st         %g1, [4+%o0]
srlx       %g1, 32, %g1

inc        4, %o1
cmp        %o1, %o2
bl         vSIL_Loop
inc        8, %o0

retl
mov        %g1,%o0


.global    vecSqrInnerLoop_s
vecSqrInnerLoop_s:

mov        0, %g1
mov        0, %g4

vSIL_Loop_s:                    
ld         [%o1], %g2  
ld         [%o0], %g5
!sll        %g5, 1, %g3
sllx       %g5, 33, %g3
srlx       %g3, 32, %g3
or         %g4, %g3, %g3
srl        %g5, 31, %g4
mulx       %g2, %g2, %g2
add        %g3, %g1, %g1
add        %g2, %g1, %g1
st         %g1, [%o0]
srlx       %g1, 32, %g1

ld         [4+%o0], %g5
!sll        %g5, 1, %g3
sllx       %g5, 33, %g3
srlx       %g3, 32, %g3
or         %g4, %g3, %g3
srl        %g5, 31, %g4
add        %g3, %g1, %g1
st         %g1, [4+%o0]
srlx       %g1, 32, %g1

inc        4, %o1
cmp        %o1, %o2
bl         vSIL_Loop_s
inc        8, %o0

retl
add        %g1,%g4,%o0



.global    vecShiftLeftByBits
vecShiftLeftByBits:
mov        %g0,%g1
sll        %o2,2,%o2
add        %o0,%o2,%o0
add        %o1,%o2,%o1
neg        %o2,%o2
mov        32,%o4
sub        %o4,%o3,%o4

vSL_Loop:                    
ld         [%o1+%o2], %g2  
sll        %g2,%o3,%g3
or         %g1, %g3, %g3
st         %g3, [%o0+%o2]
addcc      %o2,4,%o2
bnz        vSL_Loop
srl        %g2,%o4,%g1

retl
mov        %g1,%o0


.global    vecShiftRightByBits
vecShiftRightByBits:
mov        %g0,%g1
sll        %o2,2,%o2
sub        %o2,4,%o2
mov        32,%o4
sub        %o4,%o3,%o4
vSR_Loop:                    
ld         [%o1+%o2], %g2  
srl        %g2,%o3,%g3
or         %g1, %g3, %g3
st         %g3, [%o0+%o2]
subcc      %o2,4,%o2
bcc        vSR_Loop
sll        %g2,%o4,%g1
retl
mov        %g1,%o0







!------------------------------------------------------------------------------
! extern "C" digit vecMulInnerLoopi(digit *p, digit *a, digit B)
!------------------------------------------------------------------------------
! %o0 p
! %o1 a
! %o2 B
!
! %g1  carry

#define vecMulStep(i) \
ld    [i+%o1], %g2;   \
ld    [i+%o0], %g3;   \
mulx  %o2, %g2,%g2;   \
add   %g3, %g1,%g1;   \
add   %g1, %g2,%g2;   \
st    %g2, [i+%o0];   \
srlx  %g2, 32, %g1;

.global    vecMulInnerLoop1
vecMulInnerLoop1:
mov  %g0,%g1
vecMulStep(0)
st   %g1,[4+%o0]
retl
mov  %g1,%o0


.global    vecMulInnerLoop2
vecMulInnerLoop2:
mov  %g0,%g1
vecMulStep(0)
vecMulStep(4)
st   %g1,[8+%o0]
retl
mov  %g1,%o0

.global    vecMulInnerLoop3
vecMulInnerLoop3:
mov  %g0,%g1
vecMulStep(0)
vecMulStep(4)
vecMulStep(8)
st   %g1,[12+%o0]
retl
mov  %g1,%o0

.global    vecMulInnerLoop4
vecMulInnerLoop4:
mov  %g0,%g1
vecMulStep(0)
vecMulStep(4)
vecMulStep(8)
vecMulStep(12)
st   %g1, [16+%o0]
retl
mov  %g1, %o0

.global    vecMulInnerLoop5
vecMulInnerLoop5:
mov  %g0,%g1
vecMulStep(0)
vecMulStep(4)
vecMulStep(8)
vecMulStep(12)
vecMulStep(16)
st   %g1, [20+%o0]
retl
mov  %g1, %o0


.global    vecMulInnerLoop6
vecMulInnerLoop6:
mov  %g0,%g1
vecMulStep(0)
vecMulStep(4)
vecMulStep(8)
vecMulStep(12)
vecMulStep(16)
vecMulStep(20)
st   %g1, [24+%o0]
retl
mov  %g1, %o0


.global    vecMulInnerLoop7
vecMulInnerLoop7:
mov  %g0,%g1
vecMulStep(0)
vecMulStep(4)
vecMulStep(8)
vecMulStep(12)
vecMulStep(16)
vecMulStep(20)
vecMulStep(24)
st   %g1, [28+%o0]
retl
mov  %g1, %o0



.global    vecMulInnerLoop8
vecMulInnerLoop8:
mov  %g0,%g1
vecMulStep(0)
vecMulStep(4)
vecMulStep(8)
vecMulStep(12)
vecMulStep(16)
vecMulStep(20)
vecMulStep(24)
vecMulStep(28)
st   %g1, [32+%o0]
retl
mov  %g1, %o0


.global    vecMulInnerLoop9
vecMulInnerLoop9:
mov  %g0,%g1
vecMulStep(0)
vecMulStep(4)
vecMulStep(8)
vecMulStep(12)
vecMulStep(16)
vecMulStep(20)
vecMulStep(24)
vecMulStep(28)
vecMulStep(32)
st   %g1, [36+%o0]
retl
mov  %g1, %o0


.global    vecMulInnerLoop10
vecMulInnerLoop10:
mov  %g0,%g1
vecMulStep(0)
vecMulStep(4)
vecMulStep(8)
vecMulStep(12)
vecMulStep(16)
vecMulStep(20)
vecMulStep(24)
vecMulStep(28)
vecMulStep(32)
vecMulStep(36)
st   %g1, [40+%o0]
retl
mov  %g1, %o0


.global    vecMulInnerLoop11
vecMulInnerLoop11:
mov  %g0,%g1
vecMulStep(0)
vecMulStep(4)
vecMulStep(8)
vecMulStep(12)
vecMulStep(16)
vecMulStep(20)
vecMulStep(24)
vecMulStep(28)
vecMulStep(32)
vecMulStep(36)
vecMulStep(40)
st   %g1, [44+%o0]
retl
mov  %g1, %o0


.global    vecMulInnerLoop12
vecMulInnerLoop12:
mov  %g0,%g1
vecMulStep(0)
vecMulStep(4)
vecMulStep(8)
vecMulStep(12)
vecMulStep(16)
vecMulStep(20)
vecMulStep(24)
vecMulStep(28)
vecMulStep(32)
vecMulStep(36)
vecMulStep(40)
vecMulStep(44)
st   %g1, [48+%o0]
retl
mov  %g1, %o0

.global    vecMulInnerLoop13
vecMulInnerLoop13:
mov  %g0,%g1
vecMulStep(0)
vecMulStep(4)
vecMulStep(8)
vecMulStep(12)
vecMulStep(16)
vecMulStep(20)
vecMulStep(24)
vecMulStep(28)
vecMulStep(32)
vecMulStep(36)
vecMulStep(40)
vecMulStep(44)
vecMulStep(48)
st   %g1, [52+%o0]
retl
mov  %g1, %o0

.global    vecMulInnerLoop14
vecMulInnerLoop14:
mov  %g0,%g1
vecMulStep(0)
vecMulStep(4)
vecMulStep(8)
vecMulStep(12)
vecMulStep(16)
vecMulStep(20)
vecMulStep(24)
vecMulStep(28)
vecMulStep(32)
vecMulStep(36)
vecMulStep(40)
vecMulStep(44)
vecMulStep(48)
vecMulStep(52)
st   %g1, [56+%o0]
retl
mov  %g1, %o0

.global    vecMulInnerLoop15
vecMulInnerLoop15:
mov  %g0,%g1
vecMulStep(0)
vecMulStep(4)
vecMulStep(8)
vecMulStep(12)
vecMulStep(16)
vecMulStep(20)
vecMulStep(24)
vecMulStep(28)
vecMulStep(32)
vecMulStep(36)
vecMulStep(40)
vecMulStep(44)
vecMulStep(48)
vecMulStep(52)
vecMulStep(56)
st   %g1, [60+%o0]
retl
mov  %g1, %o0

.global    vecMulInnerLoop16
vecMulInnerLoop16:
mov  %g0,%g1
vecMulStep(0)
vecMulStep(4)
vecMulStep(8)
vecMulStep(12)
vecMulStep(16)
vecMulStep(20)
vecMulStep(24)
vecMulStep(28)
vecMulStep(32)
vecMulStep(36)
vecMulStep(40)
vecMulStep(44)
vecMulStep(48)
vecMulStep(52)
vecMulStep(56)
vecMulStep(60)
st   %g1, [64+%o0]
retl
mov  %g1, %o0


