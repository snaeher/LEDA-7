
%ifdef win32
%define FUNC(x) _x86_ %+ x
%else
%define FUNC(x) x86_ %+ x
%endif

segment _TEXT para public use32 class=CODE
global FUNC(vecAddCarry)
FUNC(vecAddCarry):

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
je 	_LL91
_LL92:
mov 	eax,	DWORD [ecx+edi*4]
adc 	eax,	DWORD [esi+edi*4]
mov 	DWORD [ebx+edi*4],	eax
inc 	edi
jne 	_LL92
_LL91:
mov 	eax,	0
adc 	eax,	0

;pop 	edi
;pop 	esi
;pop 	ecx
;pop 	ebx
ret


global FUNC(vecAddCarryC16)
FUNC(vecAddCarryC16):

;push 	ebx
;push 	ecx
;push 	esi
;push 	edi

;mov 	ebx,	DWORD [20+esp]
;mov 	ecx,	DWORD [24+esp]
;mov 	esi,	DWORD [28+esp]
;mov 	edi,	DWORD [32+esp]

add 	edi,	0ffffffffh

mov 	eax,	DWORD [0+ecx]
adc 	eax,	DWORD [0+esi]
mov 	DWORD [0+ebx],	eax

mov 	eax,	DWORD [4+ecx]
adc 	eax,	DWORD [4+esi]
mov 	DWORD [4+ebx],	eax

mov 	eax,	DWORD [8+ecx]
adc 	eax,	DWORD [8+esi]
mov 	DWORD [8+ebx],	eax

mov 	eax,	DWORD [12+ecx]
adc 	eax,	DWORD [12+esi]
mov 	DWORD [12+ebx],	eax

mov 	eax,	DWORD [16+ecx]
adc 	eax,	DWORD [16+esi]
mov 	DWORD [16+ebx],	eax

mov 	eax,	DWORD [20+ecx]
adc 	eax,	DWORD [20+esi]
mov 	DWORD [20+ebx],	eax

mov 	eax,	DWORD [24+ecx]
adc 	eax,	DWORD [24+esi]
mov 	DWORD [24+ebx],	eax

mov 	eax,	DWORD [28+ecx]
adc 	eax,	DWORD [28+esi]
mov 	DWORD [28+ebx],	eax

mov 	eax,	DWORD [32+ecx]
adc 	eax,	DWORD [32+esi]
mov 	DWORD [32+ebx],	eax

mov 	eax,	DWORD [36+ecx]
adc 	eax,	DWORD [36+esi]
mov 	DWORD [36+ebx],	eax

mov 	eax,	DWORD [40+ecx]
adc 	eax,	DWORD [40+esi]
mov 	DWORD [40+ebx],	eax

mov 	eax,	DWORD [44+ecx]
adc 	eax,	DWORD [44+esi]
mov 	DWORD [44+ebx],	eax

mov 	eax,	DWORD [48+ecx]
adc 	eax,	DWORD [48+esi]
mov 	DWORD [48+ebx],	eax

mov 	eax,	DWORD [52+ecx]
adc 	eax,	DWORD [52+esi]
mov 	DWORD [52+ebx],	eax

mov 	eax,	DWORD [56+ecx]
adc 	eax,	DWORD [56+esi]
mov 	DWORD [56+ebx],	eax

mov 	eax,	DWORD [60+ecx]
adc 	eax,	DWORD [60+esi]
mov 	DWORD [60+ebx],	eax

mov 	eax,	0
adc 	eax,	0

;pop 	edi
;pop 	esi
;pop 	ecx
;pop 	ebx

ret

