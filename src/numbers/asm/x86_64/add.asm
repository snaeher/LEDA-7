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

%define par1 rdi
%define par2 rsi
%define par3 rdx
%define par4 rcx

; digit vecAddCarry(digit*sum, digit *a, digit *b, digit_sz b_used)
align 16
global vecAddCarry
vecAddCarry:
; ecx = b_used

;push rbp			; save base pointer on stack
;mov rbp, rsp			;  move stack pointer to base

movsxd par4, ecx
	;  rdi = b_used, only low 32 bits are copy
                                        ; with sign extension
        ; alternative add ecx, 0 ; result in rcx is zero extended!

mov rax, par4			; save b_used
shl rax, 3			; multiply by 8 because each entry is 8 bytes		
add par1, rax			; determine end of sum
add par2, rax			;  determine end of a
add par3, rax			; determine end of b

; prepare loopwise addition, each loop will calculate
; sum[i] = a[i]+b[i]+carry
; i is stored in rdi
neg par4                         ; this can set the carry, so clear it in the next instr.
clc                             ; clear carry flag
je finish


add_digits:
mov rax, [par2+par4*8]		; load digit from a
adc rax, [par3+par4*8]		; a = a+b+carry
mov [par1+par4*8], rax      	; sum = a
inc par4				; goto next digit
jne add_digits


finish:
mov rax, 0			; clear rax
adc rax, 0			; rax will return carry


; epilogue
;pop rbp
;mov rsp, rbp			; restore stack pointer
ret
