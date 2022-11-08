#define ASM     1

.data
    STACK = 0x8400000
    IF_BIT= 0x200
    USER_CS= 0x0023
    USER_DS= 0x002B

.text
.globl context_switch_ASM
context_switch_ASM:
    movl 4(%esp),%ebx # Load entry point in EBX

    ; xorl %eax, %eax
    movl $0, %eax
    movw $USER_DS, %ax     # Push SS on stack 
    pushl %eax              
    movl  $STACK, %eax # Push ESP value for user stack
    pushl %eax
    pushfl               
    popl  %eax  # this is how we get access to contents of eflags
    orl   $IF_BIT,%eax   # set the if flag, which is     
    pushl %eax
    # Push CS on stack
    pushl $USER_CS          
    # Push EIP for user program
    pushl %ebx
# Swicth o user space
    iret