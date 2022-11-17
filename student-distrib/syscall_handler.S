#define ASM     1

; .data
;     STACK = 0x8400000
;     IF_BIT= 0x200
;     USER_CS= 0x0023
;     USER_DS= 0x002B

.text
.globl syscall_linkage, syscall_table



syscall_linkage:
  
    pushl %ebx
    pushl %ecx
    pushl %edx
    pushl %esp
    pushl %esi
    pushl %edi

    cmp $1, %eax
    jl invalid
    cmp $10, %eax # validicity check
    jg invalid
    # Push arguments on stack
    pushl %edx	# 3rd argument
    pushl %ecx	# 2nd argument
    pushl %ebx	# 1st argument
    
    call *syscall_table-4(,%eax,4) # eax is the 

    # Pop arguments of stack
    addl $12, %esp
    jmp   done

    
invalid:
    movl $-1, %eax
done:
    # Restore registers
    popl %edi
    popl %esi
    popl %esp
    popl %edx
    popl %ecx
    popl %ebx
    # Restore flags
    
    iret

syscall_table:
    .long   halt
    .long   execute
    .long   read
    .long   write
    .long   open
    .long   close
    .long   getargs
    .long   vidmap
    .long   set_handler
    .long   sigreturn
