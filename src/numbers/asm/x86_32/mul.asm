
%ifdef win32
%define FUNC(x) _ %+ x
%else
%define FUNC(x) x
%endif


%macro jmp_sse 1
mov     eax, [sse2_flag]
cmp     eax, 0
jne     %1
%endmacro


segment _TEXT para public use32 class=CODE

extern sse2_flag

align 65536

global FUNC(digMul)
FUNC(digMul):
mov 	ecx,	DWORD [12+esp]
mov 	eax,	DWORD [4+esp]
mul 	DWORD [8+esp]
mov 	DWORD [ecx],	edx
ret

extern FUNC(x86_vecMulLoopFirst)
extern FUNC(x86_vecMulInnerLoop)
extern FUNC(x86_vecSqrInnerLoop)
extern FUNC(x86_vecMulInnerLoop1)
extern FUNC(x86_vecMulInnerLoop2)
extern FUNC(x86_vecMulInnerLoop3)
extern FUNC(x86_vecMulInnerLoop4)
extern FUNC(x86_vecMulInnerLoop5)
extern FUNC(x86_vecMulInnerLoop6)
extern FUNC(x86_vecMulInnerLoop7)
extern FUNC(x86_vecMulInnerLoop8)
extern FUNC(x86_vecMulInnerLoop9)
extern FUNC(x86_vecMulInnerLoop10)
extern FUNC(x86_vecMulInnerLoop11)
extern FUNC(x86_vecMulInnerLoop12)
extern FUNC(x86_vecMulInnerLoop13)
extern FUNC(x86_vecMulInnerLoop14)
extern FUNC(x86_vecMulInnerLoop15)
extern FUNC(x86_vecMulInnerLoop16)


global FUNC(vecMulLoopFirst)
FUNC(vecMulLoopFirst):

push 	ebx
push 	ecx
push 	esi
push 	edi
push    ebp

mov 	ebx,	DWORD [24+esp]
mov 	ecx,	DWORD [28+esp]
mov 	esi,	DWORD [32+esp]
mov  	edi,	DWORD [36+esp]

jmp_sse _VMF_SSE2
call    FUNC(x86_vecMulLoopFirst)
jmp     _VMF_LEND
_VMF_SSE2 :

mov 	eax, 0
movd 	mm0, eax
movd 	mm1, edi

_Lloop:
movd 	mm2,	[ecx]
add 	ecx,	4
pmuludq mm2,	mm1
paddq 	mm0,	mm2
movd 	[ebx],	mm0
psrlq 	mm0,	32
add 	ebx,	4
cmp 	esi,	ecx
jg 	_Lloop

movd 	eax,	mm0
mov 	DWORD [ebx],	eax

emms

_VMF_LEND:

pop     ebp
pop 	edi
pop 	esi
pop 	ecx
pop 	ebx

ret


global FUNC(vecMulInnerLoop)
FUNC(vecMulInnerLoop):

push 	ebx
push 	ecx
push    esi
push    edi
push    ebp

mov 	ebx,	DWORD [24+esp]
mov 	ecx,	DWORD [28+esp]
mov 	esi,	DWORD [32+esp]
mov  	edi,	DWORD [36+esp]

jmp_sse _VMI_SSE2
call    FUNC(x86_vecMulInnerLoop)
jmp     _VMI_LEND
_VMI_SSE2:

mov 	eax,	0
movd 	mm0,	eax
movd 	mm1,	edi

_LLloop:
movd 	mm2,	[ecx]
add 	ecx,	4
movd 	mm3,	[ebx]
pmuludq mm2,	mm1
paddq 	mm3,	mm2
paddq 	mm0,	mm3
movd 	[ebx],	mm0
psrlq 	mm0,	32
add 	ebx,	4
cmp 	esi,	ecx
jg 	_LLloop

movd 	eax,	mm0
mov 	DWORD [ebx],	eax

emms

_VMI_LEND:
pop     ebp
pop     edi
pop     esi
pop 	ecx
pop 	ebx

ret


global FUNC(vecSqrInnerLoop)
FUNC(vecSqrInnerLoop):

push 	ebx
push 	ecx
push 	esi
push 	ebp

mov 	ebx,	DWORD [20+esp]
mov 	ecx,	DWORD [24+esp]
mov 	esi,	DWORD [28+esp]

jmp_sse _VSI_SSE2
call    FUNC(x86_vecSqrInnerLoop)
jmp     _VSI_LEND
_VSI_SSE2:

mov 	eax,	0
movd 	mm0,	eax


_LSloop:
movd 	mm2,	[ecx]
add 	ecx,	4
movd 	mm3,	[ebx]
pmuludq	mm2,	mm2
paddq 	mm3,	mm2
paddq 	mm0,	mm3
movd 	[ebx],	mm0
psrlq 	mm0,	32

movd 	mm3,	[4+ebx]
paddq 	mm0,	mm3
movd 	[4+ebx],mm0
psrlq 	mm0,	32

add 	ebx,	8
cmp 	esi,	ecx
jg 	_LSloop

movd 	eax,	mm0
emms

_VSI_LEND:

pop     ebp
pop     esi
pop 	ecx
pop 	ebx

ret

%macro vecMulPreamble 1
global FUNC(vecMulInnerLoop %+ %1)
FUNC(vecMulInnerLoop %+ %1):
	push 	ebx
	push 	ecx
	push 	esi
	push 	ebp
	mov 	ebx,	DWORD [20+esp]
	mov 	ecx,	DWORD [24+esp]
	mov 	esi,	DWORD [28+esp]
        jmp_sse VMI_SSE2 %+ %1
	call    FUNC(x86_vecMulInnerLoop %+ %1)
	jmp     VMI_End %+ %1
	VMI_SSE2 %+ %1 :
	mov 	eax,	0
	movd 	mm0,	eax
	movd 	mm1,	esi
%endmacro

%macro vecMulStep 1
	movd 	mm2,	[ecx+%1]
	movd 	mm3,	[ebx+%1]
	pmuludq mm2,	mm1
	paddq 	mm3,	mm2
	paddq 	mm0,	mm3
	movd 	[ebx+%1],mm0
	psrlq 	mm0,	32
%endmacro

%macro vecMulPost 2
	movd 	eax,	mm0
	mov 	DWORD [ebx+%1],	eax
	emms
	VMI_End %+ %2 :
        pop     ebp
        pop     esi
	pop 	ecx
	pop 	ebx
	ret
%endmacro

%macro define_vecMulInnerLoop 1
	vecMulPreamble %1
	%assign i 0
	%rep %1
	  vecMulStep i
	  %assign i i+4
	%endrep
	vecMulPost i, %1
%endmacro


%assign n 1
%rep 16
  define_vecMulInnerLoop n
  %assign n n+1
%endrep


