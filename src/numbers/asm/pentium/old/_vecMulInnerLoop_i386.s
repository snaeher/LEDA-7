.text 
.align 4
.globl 	 _vecMulInnerLoop_i386  

_vecMulInnerLoop_i386:
	
	 pushl	 %edi    
	 pushl	 %esi    
	 pushl	 %ebx    
	 pushl	 %ebp    

	 movl	 20(%esp),%edi     
	 movl	 24(%esp),%esi     
	 movl	 28(%esp),%ecx     
	 movl	 32(%esp),%ebp     

	 xorl	 %ebx,%ebx    
         cmpl    %esi,%ecx
         jbe     MULT_L0
	.align 4

MULT_L1:
	 movl	 (%esi),%eax    
	 mull	 %ebp     
	 addl	 %ebx,%eax    
	 adcl	 $0,%edx    
	 addl	 %eax,(%edi)   
	 adcl	 $0,%edx    
	 movl	 %edx,%ebx    
         addl    $4,%esi
         addl    $4,%edi
         cmpl    %esi,%ecx
	 ja      MULT_L1  
	 movl	 %ebx,(%edi)   

MULT_L0:
	 movl	 %ebx,%eax    

	 popl	 %ebp    
	 popl	 %ebx    
	 popl	 %esi    
	 popl	 %edi    
 	 ret



.text
.align 16
.globl Multiply_Words
Multiply_Words:

	pushl %ebp
 	movl  %esp,%ebp
 	movl 16(%ebp),%ecx # ecx := high
 	movl  8(%ebp),%eax # eax := a
 	mull 12(%ebp)      # edx:eax := a*b
 	movl %edx,(%ecx)   # high = highword(a*b)
 	movl %ebp,%esp
 	popl %ebp
 	ret
 


#.text
#.align 4
#.globl Multiply_Words
#Multiply_Words:
#	movl 28(%esp),%ecx # ecx := high
#	movl 20(%esp),%eax # eax := a
#	mull 24(%esp)      # edx:eax := a*b
#	movl %edx,(%ecx)   # high = highword(a*b)
#	ret

