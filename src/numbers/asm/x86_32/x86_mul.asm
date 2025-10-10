
%ifdef win32
%define FUNC(x) _x86_ %+ x
%else
%define FUNC(x) x86_ %+ x
%endif


segment _TEXT para public use32 class=CODE

align 16
global FUNC(digMul)
FUNC(digMul):
mov 	ecx, DWORD[12+esp]
mov 	eax, DWORD[4+esp]
mul 	DWORD[8+esp]
mov 	DWORD[ecx], 	edx
ret


global FUNC(vecMulLoopFirst)
FUNC(vecMulLoopFirst):

;push 	ebx    
;push 	ecx    
;push 	esi    
;push 	edi    
;mov 	ebx, DWORD[20+esp]
;mov 	ecx, DWORD[24+esp]
;mov 	esi, DWORD[28+esp]
;mov 	edi, DWORD[32+esp]

xor 	ebp, ebp

L1loop:
mov 	eax, DWORD[ecx]
mul 	edi     
add 	eax, ebp
adc 	edx, 0
mov 	DWORD[ebx], 	eax
mov 	ebp, edx
add 	ecx, 4
add 	ebx, 4
cmp 	esi, ecx
ja 	L1loop  

mov 	DWORD[ebx], 	ebp
mov 	eax, 	ebp

;pop 	edi    
;pop 	esi    
;pop 	ecx    
;pop 	ebx    

ret



global FUNC(vecMulInnerLoop)
FUNC(vecMulInnerLoop):

;push 	ebx    
;push 	ecx    
;push 	esi    
;push 	edi    
;mov 	ebx, DWORD[20+esp]
;mov 	ecx, DWORD[24+esp]
;mov 	esi, DWORD[28+esp]
;mov 	edi, DWORD[32+esp]

xor 	ebp, ebp

L2loop:
mov 	eax, DWORD[ecx]
mul 	edi     
add 	eax, ebp
adc 	edx, 0
add 	DWORD[ebx], eax
adc 	edx, 0
mov 	ebp, edx
add 	ecx, 4
add 	ebx, 4
cmp 	esi, ecx
ja 	L2loop 

mov 	DWORD[ebx], ebp
mov 	eax, ebp

;pop 	edi    
;pop 	esi    
;pop 	ecx    
;pop 	ebx    

ret



global FUNC(vecSqrInnerLoop)
FUNC(vecSqrInnerLoop):

;push 	ebx    
;push 	ecx    
;push 	esi    
;mov 	ebx, DWORD[16+esp]
;mov 	ecx, DWORD[20+esp]
;mov 	esi, DWORD[24+esp]

xor 	ebp, ebp

LSloop:
mov 	eax, DWORD[ecx]
add 	ecx, 4
mul 	eax     
add 	eax, ebp
adc 	edx, 0
add 	DWORD[ebx], eax
adc 	edx, 0
add 	DWORD[ebx+4],edx
mov 	ebp, 0
adc     ebp, 0

add 	ebx, 8
cmp 	esi, ecx
ja 	LSloop 

mov 	eax, ebp

;pop 	esi    
;pop 	ecx    
;pop 	ebx    

ret


%macro vecMulPreamble 1
global FUNC(vecMulInnerLoop %+ %1)
FUNC(vecMulInnerLoop %+ %1):
       xor 	ebp, ebp
%endmacro

%macro vecMulStep 1
	mov 	eax, DWORD[ecx+%1]
	mul 	esi     
	add 	eax, ebp
	adc 	edx, 0
	add 	DWORD[ebx+%1], eax
	adc 	edx, 0
	mov 	ebp, edx
%endmacro

%macro vecMulPost 1
        mov 	eax, ebp
        mov 	DWORD[ebx+%1], eax
        ret
%endmacro


vecMulPreamble 1
vecMulStep(0)
vecMulPost(4)

vecMulPreamble 2
vecMulStep(0)
vecMulStep(4)
vecMulPost(8)

vecMulPreamble 3
vecMulStep(0)
vecMulStep(4)
vecMulStep(8)
vecMulPost(12)
 
vecMulPreamble 4
vecMulStep(0)
vecMulStep(4)
vecMulStep(8)
vecMulStep(12)
vecMulPost(16)
 
vecMulPreamble 5
vecMulStep(0)
vecMulStep(4)
vecMulStep(8)
vecMulStep(12)
vecMulStep(16)
vecMulPost(20)
 
vecMulPreamble 6
vecMulStep(0)
vecMulStep(4)
vecMulStep(8)
vecMulStep(12)
vecMulStep(16)
vecMulStep(20)
vecMulPost(24)
 
vecMulPreamble 7
vecMulStep(0)
vecMulStep(4)
vecMulStep(8)
vecMulStep(12)
vecMulStep(16)
vecMulStep(20)
vecMulStep(24)
vecMulPost(28)
 
vecMulPreamble 8
vecMulStep(0)
vecMulStep(4)
vecMulStep(8)
vecMulStep(12)
vecMulStep(16)
vecMulStep(20)
vecMulStep(24)
vecMulStep(28)
vecMulPost(32)
 
vecMulPreamble 9
vecMulStep(0)
vecMulStep(4)
vecMulStep(8)
vecMulStep(12)
vecMulStep(16)
vecMulStep(20)
vecMulStep(24)
vecMulStep(28)
vecMulStep(32)
vecMulPost(36)
 
vecMulPreamble 10
vecMulStep(0)
vecMulStep(4)
vecMulStep(8)
vecMulStep(12)
vecMulStep(16)
vecMulStep(20)
vecMulStep(24)
vecMulStep(28)
vecMulStep(32)
vecMulStep(36)
vecMulPost(40)
 
vecMulPreamble 11
vecMulStep(0)
vecMulStep(4)
vecMulStep(8)
vecMulStep(12)
vecMulStep(16)
vecMulStep(20)
vecMulStep(24)
vecMulStep(28)
vecMulStep(32)
vecMulStep(36)
vecMulStep(40)
vecMulPost(44)
 
vecMulPreamble 12
vecMulStep(0)
vecMulStep(4)
vecMulStep(8)
vecMulStep(12)
vecMulStep(16)
vecMulStep(20)
vecMulStep(24)
vecMulStep(28)
vecMulStep(32)
vecMulStep(36)
vecMulStep(40)
vecMulStep(44)
vecMulPost(48)
 
vecMulPreamble 13
vecMulStep(0)
vecMulStep(4)
vecMulStep(8)
vecMulStep(12)
vecMulStep(16)
vecMulStep(20)
vecMulStep(24)
vecMulStep(28)
vecMulStep(32)
vecMulStep(36)
vecMulStep(40)
vecMulStep(44)
vecMulStep(48)
vecMulPost(52)
 
vecMulPreamble 14
vecMulStep(0)
vecMulStep(4)
vecMulStep(8)
vecMulStep(12)
vecMulStep(16)
vecMulStep(20)
vecMulStep(24)
vecMulStep(28)
vecMulStep(32)
vecMulStep(36)
vecMulStep(40)
vecMulStep(44)
vecMulStep(48)
vecMulStep(52)
vecMulPost(56)
 
vecMulPreamble 15
vecMulStep(0)
vecMulStep(4)
vecMulStep(8)
vecMulStep(12)
vecMulStep(16)
vecMulStep(20)
vecMulStep(24)
vecMulStep(28)
vecMulStep(32)
vecMulStep(36)
vecMulStep(40)
vecMulStep(44)
vecMulStep(48)
vecMulStep(52)
vecMulStep(56)
vecMulPost(60)
 
vecMulPreamble 16
vecMulStep(0)
vecMulStep(4)
vecMulStep(8)
vecMulStep(12)
vecMulStep(16)
vecMulStep(20)
vecMulStep(24)
vecMulStep(28)
vecMulStep(32)
vecMulStep(36)
vecMulStep(40)
vecMulStep(44)
vecMulStep(48)
vecMulStep(52)
vecMulStep(56)
vecMulStep(60)
vecMulPost(64)
 
