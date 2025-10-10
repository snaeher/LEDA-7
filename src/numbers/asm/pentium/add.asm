
%ifdef win32
%define FUNC(x) _ %+ x
%else
%define FUNC(x) x
%endif


segment _TEXT para public use32 class=CODE

extern FUNC(x86_vecAddCarry)
extern FUNC(x86_vecAddCarryC16)


global FUNC(vecAddCarry)
FUNC(vecAddCarry):

push 	ebx
push 	ecx
push 	esi
push 	edi

;mov     eax, 1
;cpuid

mov 	ebx,	DWORD [20+esp]
mov 	ecx,	DWORD [24+esp]
mov 	esi,	DWORD [28+esp]
mov 	edi,	DWORD [32+esp]

;and     edx, 0x04000000 
;jnz     _VAC_SSE2
;call    FUNC(x86_vecAddCarry)
;jmp     _VAC_LEND
;
;_VAC_SSE2:

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

;mov     eax, 1
;cpuid

mov 	ebx,	DWORD [20+esp]
mov 	ecx,	DWORD [24+esp]
mov 	esi,	DWORD [28+esp]
mov 	edi,	DWORD [32+esp]

;and     edx, 0x04000000 
;jnz     _VAC16_SSE2
;call    FUNC(x86_vecAddCarryC16)
;jmp    _VAC16_LEND
;
;_VAC16_SSE2

movd 	mm2,	edi

vecAddStep(0)
vecAddStep(4)
vecAddStep(8)
vecAddStep(12)
vecAddStep(16)
vecAddStep(20)
vecAddStep(24)
vecAddStep(28)
vecAddStep(32)
vecAddStep(36)
vecAddStep(40)
vecAddStep(44)
vecAddStep(48)
vecAddStep(52)
vecAddStep(56)
vecAddStep(60)

movd 	eax,	mm2
emms

_VAC16_LEND:

pop 	edi
pop 	esi
pop 	ecx
pop 	ebx
ret

