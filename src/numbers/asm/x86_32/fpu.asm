
%ifdef win32
%define FUNC(x) _ %+ x
%else
%define FUNC(x) x
%endif


%ifdef obj
segment _BSS para public use32 class=BSS
global sse2_flag
sse2_flag: resd 1
%else
common sse2_flag 2
%endif


segment _TEXT para public use32 class=CODE

align 16

global FUNC(i386_init_sse2)
FUNC(i386_init_sse2):
push 	ebx
push 	ecx
push 	esi
push 	edi
push 	edx

mov     eax, 1
cpuid
mov     eax, edx
and     eax, 0x04000000 
mov     [sse2_flag], eax

pop 	edx
pop 	edi
pop 	esi
pop 	ecx
pop 	ebx

ret


global FUNC(ieee_set_fcr)
FUNC(ieee_set_fcr):
push 	ebp
mov 	ebp,	esp
sub 	esp,	4
mov 	eax,	DWORD [8+ebp]
mov 	WORD [-2+ebp],	ax
fldcw 	[-2+ebp]
leave
ret




align 16
global FUNC(ieee_get_fcr)
FUNC(ieee_get_fcr):
push 	ebp
mov 	ebp,	esp
sub 	esp,	4
fnstcw 	[-2+ebp]
movzx 	eax,	WORD [-2+ebp]
leave
ret




align 16
global FUNC(ieee_get_fsr)
FUNC(ieee_get_fsr):
push 	ebp
mov 	ebp,	esp
sub 	esp,	4
fnstsw 	[-2+ebp]
movzx 	eax,	WORD [-2+ebp]
leave
ret



align 16
global FUNC(i386_clear_all_exceptions)
FUNC(i386_clear_all_exceptions):
push 	ebp
mov 	ebp,	esp
sub 	esp,	4
fnstsw 	[-2+ebp]
fclex
movzx 	eax,	WORD [-2+ebp]
leave
ret


 
