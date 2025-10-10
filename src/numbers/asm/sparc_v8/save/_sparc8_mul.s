
!-------------------------------------------------------------------
! digit vecMulLoopFirst(digit *p, digit *a, digit* a_stop, digit B)
!-------------------------------------------------------------------
.text
.align 4
.globl	_vecMulLoopFirst_v8
_vecMulLoopFirst_v8:

save    %sp,-256,%sp
add     %fp,-16,%g6
add     %fp,-32,%g7
xor     %g6,%g7,%o3

sll	%i3,16,%o0
srl	%o0,16,%o0
st	%o0,[%g6]
ld	[%g6],%f1
fitod	%f1,%f4
srl	%i3,16,%o0
st	%o0,[%g6]
ld	[%g6],%f1
fitod	%f1,%f6

mov     0, %g3
st      %g3,[%g6]
ld	[%g6],%f0

ld	[%i1],%f1
fxtod	%f0,%f2
fmuld	%f2,%f6,%f10
fmuld	%f2,%f4,%f8
fdtox	%f10,%f14
std	%f14,[%g6]
ldx	[%g6],%g2		
fdtox	%f8,%f12
std	%f12,[%g6]
ldx	[%g6],%g1		
sllx	%g2,16,%g2		
add	%g2,%g1,%g1		
inc	4,%i1		


ld	[%i1],%f1
fxtod	%f0,%f2
fmuld	%f2,%f6,%f10
fmuld	%f2,%f4,%f8
fdtox	%f10,%f14
std	%f14,[%g7-8]
fdtox	%f8,%f12
std	%f12,[%g7]
inc	4,%i1		

cmp     %i1,%i2
be      Lend2
nop

ld	[%i1],%f1
fxtod	%f0,%f2
fmuld	%f2,%f6,%f10
fmuld	%f2,%f4,%f8
fdtox	%f10,%f14
std	%f14,[%g6-8]
fdtox	%f8,%f12
std	%f12,[%g6]
inc	4,%i1		

ld	[%i1],%f1
inc	4,%i1		
inc     4,%i2

L1:
cmp     %i1,%i2
be      L2

fxtod	%f0,%f2
ld	[%i1],%f1
inc	4,%i1		
add	%g3,%g1,%g4		
srlx	%g4,32,%g3
ldx	[%g7-8],%g2		
fmuld	%f2,%f6,%f10
ldx	[%g7],%g1		
fmuld	%f2,%f4,%f8
sllx	%g2,16,%g2		
st	%g4,[%i0]
fdtox	%f10,%f14
add	%g2,%g1,%g1		
std	%f14,[%g7-8]
fdtox	%f8,%f12
std	%f12,[%g7]
inc	4,%i0		

cmp     %i1,%i2
be      L3

fxtod	%f0,%f2
ld	[%i1],%f1
inc	4,%i1		
add	%g3,%g1,%g4		
srlx	%g4,32,%g3
ldx	[%g6-8],%g2		
fmuld	%f2,%f6,%f10
ldx	[%g6],%g1		
fmuld	%f2,%f4,%f8
sllx	%g2,16,%g2		
st	%g4,[%i0]
fdtox	%f10,%f14
add	%g2,%g1,%g1		
std	%f14,[%g6-8]
fdtox	%f8,%f12
std	%f12,[%g6]
inc	4,%i0		

b,a     L1

L2:
xor     %g6,%o3,%g6
xor     %g7,%o3,%g7
L3:
add	%g3,%g1,%g4		
srlx	%g4,32,%g3
ldx	[%g6-8],%g2		
ldx	[%g6],%g1		
sllx	%g2,16,%g2		
st	%g4,[%i0]
add	%g2,%g1,%g1		
inc	4,%i0		

Lend2:
add	%g3,%g1,%g4		
srlx	%g4,32,%g3
ldx	[%g7-8],%g2		
ldx	[%g7],%g1		
sllx	%g2,16,%g2		
st	%g4,[%i0]
add	%g2,%g1,%g1		
inc	4,%i0		

add	%g3,%g1,%g4		
srlx	%g4,32,%g3
st	%g4,[%i0]
st	%g3,[%i0+4]
ret
restore %g0,%g3,%o0		



!--------------------------------------------------------------
! digit vecMulLoop(digit *p, digit *a, digit* a_stop, digit B)
!--------------------------------------------------------------
.text
.align 4
.globl	_vecMulLoop_v8
_vecMulLoop_v8:

save    %sp,-256,%sp
add     %fp,-16,%g6
add     %fp,-32,%g7
xor     %g6,%g7,%o3

sll	%i3,16,%o0
srl	%o0,16,%o0
st	%o0,[%g6]
ld	[%g6],%f1
fitod	%f1,%f4
srl	%i3,16,%o0
st	%o0,[%g6]
ld	[%g6],%f1
fitod	%f1,%f6


mov     0, %g3
st      %g3,[%g6]
ld	[%g6],%f0

ld	[%i1],%f1
fxtod	%f0,%f2
fmuld	%f2,%f6,%f10
fmuld	%f2,%f4,%f8
fdtox	%f10,%f14
std	%f14,[%g6]
ldx	[%g6],%g2		
fdtox	%f8,%f12
std	%f12,[%g6]
ldx	[%g6],%g1		
sllx	%g2,16,%g2		
add	%g2,%g1,%g1		
inc	4,%i1		

ld	[%i1],%f1
fxtod	%f0,%f2
fmuld	%f2,%f6,%f10
fmuld	%f2,%f4,%f8
fdtox	%f10,%f14
std	%f14,[%g7-8]
fdtox	%f8,%f12
std	%f12,[%g7]
inc	4,%i1		

cmp     %i1,%i2
be      LLend2
nop

ld	[%i1],%f1
fxtod	%f0,%f2
fmuld	%f2,%f6,%f10
fmuld	%f2,%f4,%f8
fdtox	%f10,%f14
std	%f14,[%g6-8]
fdtox	%f8,%f12
std	%f12,[%g6]
inc	4,%i1		

ld	[%i0],%g5
ld	[%i1],%f1

inc	4,%i1		
inc     4,%i2

LL1:
cmp     %i1,%i2
be      LL2

fxtod	%f0,%f2
ld	[%i1],%f1
inc	4,%i1		
add	%g5,%g1,%g1		
add	%g3,%g1,%g4		
ld	[%i0+4],%g5
srlx	%g4,32,%g3
ldx	[%g7-8],%g2		
fmuld	%f2,%f6,%f10
ldx	[%g7],%g1		
fmuld	%f2,%f4,%f8
sllx	%g2,16,%g2		
st	%g4,[%i0]
fdtox	%f10,%f14
add	%g2,%g1,%g1		
std	%f14,[%g7-8]
fdtox	%f8,%f12
std	%f12,[%g7]
inc	4,%i0		

cmp     %i1,%i2
be      LL3

fxtod	%f0,%f2
ld	[%i1],%f1
inc	4,%i1		
add	%g5,%g1,%g1		
add	%g3,%g1,%g4		
ld	[%i0+4],%g5
srlx	%g4,32,%g3
ldx	[%g6-8],%g2		
fmuld	%f2,%f6,%f10
ldx	[%g6],%g1		
fmuld	%f2,%f4,%f8
sllx	%g2,16,%g2		
st	%g4,[%i0]
fdtox	%f10,%f14
add	%g2,%g1,%g1		
std	%f14,[%g6-8]
fdtox	%f8,%f12
std	%f12,[%g6]
inc	4,%i0		

b,a     LL1

LL2:
xor     %g6,%o3,%g6
xor     %g7,%o3,%g7

LL3:
ld	[%i0],%g5
add	%g5,%g1,%g1		
add	%g3,%g1,%g4		
srlx	%g4,32,%g3
ldx	[%g6-8],%g2		
ldx	[%g6],%g1		
sllx	%g2,16,%g2		
st	%g4,[%i0]
add	%g2,%g1,%g1		
inc	4,%i0		

LLend2:

ld	[%i0],%g5
add	%g5,%g1,%g1		
add	%g3,%g1,%g4		
srlx	%g4,32,%g3
ldx	[%g7-8],%g2		
ldx	[%g7],%g1		
sllx	%g2,16,%g2		
st	%g4,[%i0]
add	%g2,%g1,%g1		
inc	4,%i0		

ld	[%i0],%g5
add	%g5,%g1,%g1		
add	%g3,%g1,%g4		
srlx	%g4,32,%g3
st	%g4,[%i0]
st	%g3,[%i0+4]
ret
restore %g0,%g3,%o0		



!-----------------------------------------------------------------------------
! extern "C" digit digMul(digit a, digit b, digit* high);
! (returns lower 32 bit of product, stores higher 32 bit in *high)
!------------------------------------------------------------------------------

.text
.align 4
.globl _digMul_v8
_digMul_v8:	
umul %o0,%o1,%o0
rd   %y,%g1
retl
st   %g1,[%o2]

	

