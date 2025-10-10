
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


align 16
global ieee_set_fcr
ieee_set_fcr:
push rbp
mov rbp, rsp
sub rsp, 8
xor rax,rax
add rax, par1
mov DWORD [-4+rbp], eax
ldmxcsr [-4+rbp]
leave
ret

align 16
global ieee_get_fcr
ieee_get_fcr:
push rbp
mov rbp, rsp
sub rsp, 8
stmxcsr [-4+rbp]
movzx rax, WORD[-4+rbp]
leave
ret

align 16
global ieee_set_fsr
ieee_set_fsr:
push rbp
mov rbp, rsp
sub rsp, 8
xor rax, rax
add rax, par1
mov DWORD [-4+rbp], eax
ldmxcsr [-4+rbp]
leave
ret

align 16
global ieee_get_fsr
ieee_get_fsr:
push rbp
mov rbp, rsp
sub rsp, 8
stmxcsr [-4+rbp]
movzx rax, WORD[-4+rbp]
leave
ret
push rbp

