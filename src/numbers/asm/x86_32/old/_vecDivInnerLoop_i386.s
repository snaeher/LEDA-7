.text 
.align  4
.globl   _vecDivInnerLoop_i386  
_vecDivInnerLoop_i386:
	
         pushl %edi    
         pushl %esi    
         pushl %ebx    
         pushl %ebp    

         movl  20(%esp),%edi     
         movl  24(%esp),%esi     
         movl  28(%esp),%ecx     
         movl  32(%esp),%ebp     

	 xorl  %ebx,%ebx    

         cmpl  %esi,%ecx
         jbe   DIV_L0

        .align 4
DIV_L1:
         movl  (%esi),%eax    
         mull  %ebp     
         addl  %ebx, %eax    
         adcl  $0,%edx    
         subl  %eax,(%edi)   
         adcl  $0,%edx    
         movl  %edx,%ebx    
         addl  $4,%esi
         addl  $4,%edi
         cmpl  %esi,%ecx
	 ja    DIV_L1  

	 subl  %ebx,(%edi)   

DIV_L0:
         movl $0,%eax
         adcl $0,%eax

         popl  %ebp    
         popl  %ebx    
         popl  %esi    
         popl  %edi    
         ret
