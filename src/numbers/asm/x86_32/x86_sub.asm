
%ifdef win32
%define FUNC(x) _x86_ %+ x
%else
%define FUNC(x) x86_ %+ x
%endif


segment _TEXT para public use32 class=CODE

global FUNC(vecSubCarry)
FUNC(vecSubCarry):

;push 	ebx
;push 	ecx
;push 	esi
;push 	edi

;mov 	ebx,	DWORD [20+esp]
;mov 	ecx,	DWORD [24+esp]
;mov 	esi,	DWORD [28+esp]
;mov 	edi,	DWORD [32+esp]

mov 	eax,	edi
shl 	eax,	2
add 	ebx,	eax
add 	ecx,	eax
add 	esi,	eax

neg 	edi			
add 	edi,	0
je 	_L91
_L92:
mov 	eax,	DWORD [ecx+edi*4]
sbb 	eax,	DWORD [esi+edi*4]
mov 	DWORD [ebx+edi*4],	eax
inc 	edi
jne 	_L92
_L91:
mov 	eax,	0
adc 	eax,	0

;pop 	edi
;pop 	esi
;pop 	ecx
;pop 	ebx

ret

