#define ASM     1

; .data
;     STACK = 0x8400000
;     IF_BIT= 0x200
;     USER_CS= 0x0023
;     USER_DS= 0x002B

.text
.globl syscall_linkage, syscall_table



syscall_linkage:
    PUSHFL # first, push eflags and all registers except eax
    pushl %ebx
    pushl %ecx
    pushl %edx
    pushl %esp
    pushl %ebp
    pushl %esi
    pushl %edi

    cmpl $1, %eax
    jb invalid
    cmpl $10, %eax # validicity check
    ja invalid
    # Push arguments on stack
    pushl %edx	# 3rd argument
    pushl %ecx	# 2nd argument
    pushl %ebx	# 1st argument
    
    call *syscall_table-4(,%eax,4) # eax is the 
    
    # Pop arguments of stack
    addl $12, %esp
invalid:
    # Restore registers
    popl %edi
    popl %esi
    popl %ebp
    popl %esp
    popl %edx
    popl %ecx
    popl %ebx
    # Restore flags
    popfl
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
