
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

extern FUNC(x86_vecDivInnerLoop)

extern sse2_flag

align 65536
global FUNC(vecDivInnerLoop)
FUNC(vecDivInnerLoop):

push 	ebx
push 	ecx
push 	esi
push 	edi
push    ebp

mov 	ebx,	DWORD [24+esp]
mov 	ecx,	DWORD [28+esp]
mov 	esi,	DWORD [32+esp]
mov 	edi,	DWORD [36+esp]

jmp_sse _LL_SSE2
call    FUNC(x86_vecDivInnerLoop)
jmp     _LL_End
_LL_SSE2:

movd 	mm2,	edi
mov 	eax,	0ffffffffh
movd 	mm0,	eax
movd 	mm1,	eax
psllq 	mm1,	32
psubq 	mm1,	mm0

_LLloop:
movd 	mm3,	[ecx]
movd 	mm4,	[ebx]
paddq 	mm4,	mm1
pmuludq mm3,	mm2
psubq 	mm4,	mm3
paddq 	mm0,	mm4
movd 	[ebx],	mm0
psrlq 	mm0,	32
add 	ecx,	4
add 	ebx,	4
cmp 	esi,	ecx
jg 	_LLloop

movd 	eax,	mm0
not 	eax
sub 	DWORD [ebx],	eax

mov 	eax,	0
adc 	eax,	0

emms

_LL_End:

pop 	ebp
pop 	edi
pop 	esi
pop 	ecx
pop 	ebx

ret


 
