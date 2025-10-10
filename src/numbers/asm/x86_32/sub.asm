
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

align 256

extern FUNC(x86_vecSubCarry)

global FUNC(vecSubCarry)
FUNC(vecSubCarry):

push 	ebx
push 	ecx
push 	esi
push 	edi

mov 	ebx, DWORD [20+esp]
mov 	ecx, DWORD [24+esp]
mov 	esi, DWORD [28+esp]
mov 	edi, DWORD [32+esp]

jmp_sse _VSC_SSE2
call    FUNC(x86_vecSubCarry)
jmp     _VSC_LEND
_VSC_SSE2:

mov 	eax, 1
movd 	mm2, eax
mov 	eax, 0xffffffff
movd 	mm7, eax

mov     eax, edi
shl 	eax, 2
add 	ebx, eax
add 	ecx, eax
add 	esi, eax
neg 	eax			


_VSC_L1:
movd 	mm0, [ecx+eax]
movd  	mm1, [esi+eax]
paddq   mm0, mm7
psubq   mm0, mm1
paddq 	mm2, mm0
movd 	[ebx+eax], mm2
psrlq 	mm2, 32
add 	eax, 4
jnz     _VSC_L1

movd 	eax, mm2
xor     eax, 1

emms

_VSC_LEND:

pop 	edi
pop 	esi
pop 	ecx
pop 	ebx
ret






global FUNC(vecSubCarry0)
FUNC(vecSubCarry0):
push 	ebx
push 	ecx
push 	edx

mov 	eax,	0
movd 	mm0,	eax

mov 	ebx,	DWORD [16+esp]
mov 	ecx,	DWORD [20+esp]
mov 	edx,	DWORD [24+esp]
mov 	eax,	DWORD [28+esp]

shl 	eax,	2
add 	ebx,	eax
add 	ecx,	eax
add 	edx,	eax
neg 	eax			

_VSC_L11:

movd 	mm1,	[edx+eax]
paddq 	mm1,	mm0
movd 	mm0,	[ecx+eax]
psubq 	mm0,	mm1
movd 	[ebx+eax],	mm0
psrlq 	mm0,	63
add 	eax, 4
jnz 	_VSC_L11

movd 	eax,	mm0

emms
pop 	edx
pop 	ecx
pop 	ebx
ret


 
