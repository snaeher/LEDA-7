
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

extern FUNC(x86_vecAddCarry)
extern FUNC(x86_vecAddCarryC16)

extern sse2_flag

global FUNC(vecAddCarry)
FUNC(vecAddCarry):

push 	ebx
push 	ecx
push 	esi
push 	edi

mov 	ebx,	DWORD [20+esp]
mov 	ecx,	DWORD [24+esp]
mov 	esi,	DWORD [28+esp]
mov 	edi,	DWORD [32+esp]

jmp_sse _VAC_SSE2
call    FUNC(x86_vecAddCarry)
jmp     _VAC_LEND
_VAC_SSE2:

mov 	eax, 0
movd 	mm0, eax

mov 	eax, edi

shl 	eax, 2
add 	ebx, eax
add 	ecx, eax
add 	esi, eax
neg 	eax			

_VAC_L1:
movd 	mm1, [ecx+eax]
movd 	mm2, [esi+eax]
paddq 	mm1, mm2
paddq 	mm0, mm1
movd 	[ebx+eax], mm0
psrlq 	mm0, 32
add  	eax, 4
jnz 	_VAC_L1

movd 	eax, mm0
emms

_VAC_LEND:

pop 	edi
pop 	esi
pop 	ecx
pop 	ebx
ret



%macro vecAddStep 1
   movd 	mm0,	[ecx+%1]
   movd 	mm1,	[edx+%1]
   paddq 	mm0,	mm1
   paddq 	mm2,	mm0
   movd 	[ebx+%1],	mm2
   psrlq 	mm2,	32
%endmacro



global FUNC(vecAddCarryC16)
FUNC(vecAddCarryC16)

push 	ebx
push 	ecx
push 	esi
push 	edi

mov 	ebx,	DWORD [20+esp]
mov 	ecx,	DWORD [24+esp]
mov 	esi,	DWORD [28+esp]
mov 	edi,	DWORD [32+esp]

jmp_sse _VAC16_SSE2
call    FUNC(x86_vecAddCarryC16)
jmp    _VAC16_LEND
_VAC16_SSE2

movd 	mm2,	edi

%assign i 0
%rep 16
  vecAddStep i
  %assign i i+4
%endrep

movd 	eax,	mm2
emms

_VAC16_LEND:

pop 	edi
pop 	esi
pop 	ecx
pop 	ebx
ret

