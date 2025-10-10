
%ifdef win32
%define FUNC(x) _x86_ %+ x
%else
%define FUNC(x) x86_ %+ x
%endif



segment _TEXT para public use32 class=CODE
align 16

global FUNC(vecDivInnerLoop)
FUNC(vecDivInnerLoop):

;push 	ebx    
;push 	ecx    
;push 	esi    
;push 	edi    

;mov 	ebx,	DWORD [20+esp]
;mov 	ecx,	DWORD [24+esp]
;mov 	esi,	DWORD [28+esp]
;mov 	edi,	DWORD [32+esp]

xor 	ebp,	ebp

DIV_L1:	
mov 	eax,	DWORD [ecx]
mul 	edi     
add 	eax,	ebp
adc 	edx,	0
sub 	DWORD [ebx],	eax
adc 	edx,	0
mov 	ebp,	edx
add 	ecx,	4
add 	ebx,	4
cmp 	esi,	ecx
ja 	DIV_L1  

sub 	DWORD [ebx],	ebp

mov 	eax,	0
adc 	eax,	0

;pop 	edi    
;pop 	esi    
;pop 	ecx    
;pop 	ebx    

ret
 
