
%ifdef win32
%define FUNC(x) _ %+ x
%else
%define FUNC(x) x
%endif



segment _TEXT para public use32 class=CODE

align 16
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


 
