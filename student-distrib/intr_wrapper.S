#define ASM     1

#include "intr_wrapper.h"

.globl pit_wrapper, keyb_wrapper, rtc_wrapper
pit_wrapper:
    pushal                          # store all the registers
    pushfl
    call pit_intr_handler           # call the interrupt handler                 
    popfl      
    popal                           # restore the regesters
    iret

keyb_wrapper:
    pushal                          # store all the registers
    pushfl
    call keyb_intr_handler       # call the interrupt handler                
    popfl      
    popal                           # restore the regesters
    iret

rtc_wrapper:
    pushal                          # store all the registers
    pushfl
    call rtc_intr_handler       # call the interrupt handler      
    popfl      
    popal                           # restore the regesters
    iret