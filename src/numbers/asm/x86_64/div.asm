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

; digit vecDivInnerLoop(digit* p, digit* a, digit* a_stop, digit B)

global vecDivInnerLoop
vecDivInnerLoop:
;save register
push r12

xor r10,r10                       ; r10=c = 0 at the beginning
mov r11, rdx                      ; save par3 in tmp. register

xor r10,r10             ; carry=0
mov r11, par3           ; save par in temp. register
DIVL1:
cmp par2, r11
je DIVL2
mov r12, QWORD[par1]    ; P=*p;
mov rax, QWORD[par2]    ; load *a
mul par4                ; rdx:rax = *a * B;
sub r12, rax            ; P -= low mult. result
adc rdx, 0              ; possibly add borrow in subtraction to high
sub r12, r10           ; P-= carry
adc rdx, 0              ; possibly add carry in addition to high
mov r10, rdx            ; carry = high
mov QWORD[par1], r12    ; *p = P
add par1, 8             ; p++
add par2, 8             ; a++
jmp DIVL1                  ; end loop

DIVL2:
sub QWORD[par1], r10    ; P = *p; *p=P-carry ;
mov rax, 0
adc rax, 0              ; return P < carry (sub instruction has implicitely performed comparison!)
pop r12
ret
