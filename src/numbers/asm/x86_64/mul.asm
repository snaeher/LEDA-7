; Routines for multiplication of integers

%define par1 rdi
%define par2 rsi
%define par3 rdx
%define par4 rcx

%ifidn __OUTPUT_FORMAT__,elf
section .note.GNU-stack noalloc noexec nowrite progbits
%endif
%ifidn __OUTPUT_FORMAT__,elf32
section .note.GNU-stack noalloc noexec nowrite progbits
%endif
%ifidn __OUTPUT_FORMAT__,elf64
section .note.GNU-stack noalloc noexec nowrite progbits
%endif



[section .text]
bits 64

; digit digMul digit a, digit b, digit* high 


global digMul
digMul:
mov r11, par3                  ; save par3 in temp. register
mov rax, par1                  ; load parameter a to rax
mul par2                       ; multiply by b
;clc                            ; mul can set CF, clear it before return
mov QWORD[r11], rdx           ; store upper 64 bit mult. result in *high
ret



; digit vecMulLoopFirst digit* p, digit* a, digit* a_stop, digit B 

global vecMulLoopFirst
vecMulLoopFirst:

;push rbp                        ; prologue, save base pointer
;mov rbp, rsp


xor r10,r10                       ; r10=c = 0 at the beginning
mov r11, rdx                      ; save par3 in tmp. register
L1:
cmp par2, r11                    ; test if a == a_stop
jge L2
mov rax, QWORD[par2]             ; load *a
mul par4                         ;rdx:rax = *a * B  P=rax,high=rdx 

add rax, r10                     ; add c to low result 
adc rdx, 0                      ; add carry to high result
mov r10, rdx                     ; c = high
mov QWORD[par1], rax             ; *p=P
add par1, 8                      ; p++
add par2, 8                       ; a++
jmp L1


L2:
mov QWORD[par1], r10              ; *P = c
mov rax, r10                     ; prepare return value

;epilogue
;pop rbp
;mov rsp, rbp
ret

;extern "C" digit vecMulInnerLoop digit* p, digit* a, digit* a_stop, digit B 
global vecMulInnerLoop
vecMulInnerLoop:
; r10 stores carry
; r12 : variable P

;save registers
push r12

xor r10,r10             ; carry=0
mov r11, par3           ; save par in temp. register
L3:
cmp par2, r11
jge L4
mov r12, QWORD[par1]    ; P=*p;
mov rax, QWORD[par2]    ; load *a
mul par4                ; rdx:rax = *a * B;
add r12, rax            ; P += low mult. result
adc rdx, 0              ; possibly add carry in addition to high
add r12 ,r10           ; P+= carry
adc rdx, 0              ; possibly add carry in addition to high
mov r10, rdx            ; carry = high
mov QWORD[par1], r12    ; *p = P
add par1, 8             ; p++
add par2, 8             ; a++
jmp L3                   ; end loop

L4:
mov QWORD[par1], r10    ; *p = carry 
mov rax, r10            ; return value is *p = carry

; restore registers
pop r12
ret


; this is the inner loop of vecMulInnerLoop
%macro vecMulStep 1

mov r12, QWORD[par1 + %1]    ; P=*p;
mov rax, QWORD[par2 + %1] ; load *a
mul par3                   ; multiply by B
add r12, rax            ; P += low mult. result
adc rdx, 0              ; possibly add carry in addition to high
add r12 ,r10           ; P+= carry
adc rdx, 0              ; possibly add carry in addition to high
mov r10, rdx            ; carry = high
mov QWORD[par1 + %1], r12    ; *p = P

%endmacro


; we do loop unrolling in assembler

global vecMulInnerLoop1
vecMulInnerLoop1:

;save registers
 push r12

xor r10,r10             ; carry=0

vecMulStep 0

mov QWORD[par1+8], r10    
mov rax, r10            

; restore registers
pop r12
ret

global vecMulInnerLoop2
vecMulInnerLoop2:

;save registers
 push r12

xor r10,r10             ; carry=0

vecMulStep 0
vecMulStep 8

mov QWORD[par1+16], r10    
mov rax, r10            

; restore registers
pop r12
ret

global vecMulInnerLoop3
vecMulInnerLoop3:

;save registers
 push r12

xor r10,r10             ; carry=0

vecMulStep 0 
vecMulStep 8 
vecMulStep 16 

mov QWORD[par1+24], r10    
mov rax, r10            

; restore registers
pop r12
ret

global vecMulInnerLoop4
vecMulInnerLoop4:

;save registers
 push r12

xor r10,r10             ; carry=0

vecMulStep 0 
vecMulStep 8 
vecMulStep 16 
vecMulStep 24 

mov QWORD[par1+32], r10    
mov rax, r10            

; restore registers
pop r12
ret

global vecMulInnerLoop5
vecMulInnerLoop5:

;save registers
 push r12

xor r10,r10             ; carry=0

vecMulStep 0 
vecMulStep 8 
vecMulStep 16 
vecMulStep 24 
vecMulStep 32 

mov QWORD[par1+40], r10    
mov rax, r10            

; restore registers
pop r12
ret

global vecMulInnerLoop6
vecMulInnerLoop6:

;save registers
 push r12

xor r10,r10             ; carry=0

vecMulStep 0 
vecMulStep 8 
vecMulStep 16 
vecMulStep 24 
vecMulStep 32 
vecMulStep 40 

mov QWORD[par1+48], r10    
mov rax, r10            

; restore registers
pop r12
ret


global vecMulInnerLoop7
vecMulInnerLoop7:

;save registers
 push r12

xor r10,r10             ; carry=0

vecMulStep 0 
vecMulStep 8 
vecMulStep 16 
vecMulStep 24 
vecMulStep 32 
vecMulStep 40 
vecMulStep 48 


mov QWORD[par1+56], r10    
mov rax, r10            

; restore registers
pop r12
ret

global vecMulInnerLoop8
vecMulInnerLoop8:

;save registers
 push r12

xor r10,r10             ; carry=0

vecMulStep 0 
vecMulStep 8 
vecMulStep 16 
vecMulStep 24 
vecMulStep 32 
vecMulStep 40 
vecMulStep 48 
vecMulStep 56 


mov QWORD[par1+64], r10    
mov rax, r10            

; restore registers
pop r12
ret

global vecMulInnerLoop9
vecMulInnerLoop9:

;save registers
 push r12

xor r10,r10             ; carry=0

vecMulStep 0 
vecMulStep 8 
vecMulStep 16 
vecMulStep 24 
vecMulStep 32 
vecMulStep 40 
vecMulStep 48 
vecMulStep 56 
vecMulStep 64 


mov QWORD[par1+72], r10    
mov rax, r10            

; restore registers
pop r12
ret

global vecMulInnerLoop10
vecMulInnerLoop10:

;save registers
 push r12

xor r10,r10             ; carry=0

vecMulStep 0 
vecMulStep 8 
vecMulStep 16 
vecMulStep 24 
vecMulStep 32 
vecMulStep 40 
vecMulStep 48 
vecMulStep 56 
vecMulStep 64 
vecMulStep 72 
 

mov QWORD[par1+80], r10    
mov rax, r10            

; restore registers
pop r12
ret

global vecMulInnerLoop11
vecMulInnerLoop11:

;save registers
 push r12

xor r10,r10             ; carry=0

vecMulStep 0 
vecMulStep 8 
vecMulStep 16 
vecMulStep 24 
vecMulStep 32 
vecMulStep 40 
vecMulStep 48 
vecMulStep 56 
vecMulStep 64 
vecMulStep 72 
vecMulStep 80 


mov QWORD[par1+88], r10    
mov rax, r10            

; restore registers
pop r12
ret

global vecMulInnerLoop12
vecMulInnerLoop12:

;save registers
 push r12

xor r10,r10             ; carry=0

vecMulStep 0 
vecMulStep 8 
vecMulStep 16 
vecMulStep 24 
vecMulStep 32 
vecMulStep 40 
vecMulStep 48 
vecMulStep 56 
vecMulStep 64 
vecMulStep 72 
vecMulStep 80 
vecMulStep 88 


mov QWORD[par1+96], r10    
mov rax, r10            

; restore registers
pop r12
ret

global vecMulInnerLoop13
vecMulInnerLoop13:

;save registers
 push r12

xor r10,r10             ; carry=0

vecMulStep 0 
vecMulStep 8 
vecMulStep 16 
vecMulStep 24 
vecMulStep 32 
vecMulStep 40 
vecMulStep 48 
vecMulStep 56 
vecMulStep 64 
vecMulStep 72 
vecMulStep 80 
vecMulStep 88 
vecMulStep 96 


mov QWORD[par1+104], r10    
mov rax, r10            

; restore registers
pop r12
ret

global vecMulInnerLoop14
vecMulInnerLoop14:

;save registers
 push r12

xor r10,r10             ; carry=0

vecMulStep 0 
vecMulStep 8 
vecMulStep 16 
vecMulStep 24 
vecMulStep 32 
vecMulStep 40 
vecMulStep 48 
vecMulStep 56 
vecMulStep 64 
vecMulStep 72 
vecMulStep 80 
vecMulStep 88 
vecMulStep 96 
vecMulStep 104 


mov QWORD[par1+112], r10    
mov rax, r10            

; restore registers
pop r12
ret

global vecMulInnerLoop15
vecMulInnerLoop15:

;save registers
 push r12

xor r10,r10             ; carry=0

vecMulStep 0 
vecMulStep 8 
vecMulStep 16 
vecMulStep 24 
vecMulStep 32 
vecMulStep 40 
vecMulStep 48 
vecMulStep 56 
vecMulStep 64 
vecMulStep 72 
vecMulStep 80 
vecMulStep 88 
vecMulStep 96 
vecMulStep 104 
vecMulStep 112 


mov QWORD[par1+120], r10    
mov rax, r10            

; restore registers
pop r12
ret

global vecMulInnerLoop16
vecMulInnerLoop16:

;save registers
 push r12

xor r10,r10             ; carry=0

vecMulStep 0 
vecMulStep 8 
vecMulStep 16 
vecMulStep 24 
vecMulStep 32 
vecMulStep 40 
vecMulStep 48 
vecMulStep 56 
vecMulStep 64 
vecMulStep 72 
vecMulStep 80 
vecMulStep 88 
vecMulStep 96 
vecMulStep 104 
vecMulStep 112 
vecMulStep 120 


mov QWORD[par1+128], r10    
mov rax, r10            

; restore registers
pop r12
ret




