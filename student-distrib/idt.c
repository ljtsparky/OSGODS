#include "idt.h"
#include "lib.h"
#include "keyboard.h"
#include "rtc.h"
void* exception_handler_table[20] = {
    &DIVIDE_ERROR_EXC_HANDLER,
    &DEBUG_EXC_HANDLER,
    &NMI_INTERRUPT_HANDLER,
    &BREAKPOINT_EXC_HANDLER,
    &OVERFLOW_EXC_HANDLER,
    &BOUND_RANGE_EXCEEDED_EXC_HANDLER,
    &INVALID_OPCODE_EXC_HANDLER,
    &DEVICE_NOT_AVAILABLE_EXC_HANDLER,
    &DOUBLE_FAULT_EXC_HANDLER,
    &COPROCESSOR_SEGMENT_OVERRUN_HANDLER,
    &INVALID_TSS_EXC_HANDLER,
    &SEGMENT_NOT_PRESENT_HANDLER,
    &STACK_FAULT_EXCEPTION_HANDLER,
    &GENERAL_PROTECTION_EXC_HANDLER,
    &PAGE_FAULT_EXC_HANDLER,
    &PRESERVED_HANDLER,
    &X87_FPU_FLOATING_POINT_ERROR_HANDLER,
    &ALIGNMENT_CHECK_EXC_HANDLER,
    &MACHINE_CHECK_EXC_HANDLER,
    &SIMD_FLOATING_POINT_EXC_HANDLER
};

void init_idt()
{
    /*int iterator;
    for (iterator = 0; iterator < 0x20; iterator++)
    { // first 32 are designed by intel and only the first twenty are defined, last 12 are reserved
        idt[iterator].reserved3;
    }
    for (iterator=0x20;iterator<0xFF;iterator++)
    { //fill the rest of the vectors including 
        
    }*/
    printf("try initialze idt \n");
    int i;
    for ( i = 0; i < NUM_VEC; i++)      // NUM_VEC is the idt size 256
    {
        idt[i].seg_selector = KERNEL_CS;
        idt[i].reserved4 = 0;
        // will be updated if it is interrupt gate
        idt[i].reserved3 = 1;
        idt[i].reserved2 = 1;
        idt[i].reserved1 = 1;
        idt[i].reserved0 = 0;
        idt[i].size = 1;
        idt[i].reserved0 = 0; 

        // below two properties will be updated 
        idt[i].dpl = 0;     
        idt[i].present = 0; 
        
    }
    for (i=0; i<20; i++)    {
        SET_IDT_ENTRY(idt[i], exception_handler_table[i]);
    }
    for (i=20; i<0x20; i++) 
    {
        SET_IDT_ENTRY(idt[i], exception_handler_table[15]);
    }

    SET_IDT_ENTRY(idt[0x21], &keyb_intr_handler);       // keboard interrupt 
    idt[0x21].reserved3 = 0;
    // SET_IDT_ENTRY(idt[0x28], &rtc_intr_handler);     // rtc interrupt
    // idt[0x28].reserved3 = 0;
    printf("finished initialze idt \n");
}
/* DIVIDE_ERROR_EXC_HANDLER:
 *  description: handle the DIVIDE_ERROR exception
 *  input: none 
 *  output: none
 */
void DIVIDE_ERROR_EXC_HANDLER()
{
    printf("DIVIDE_ERROR_EXC_HANDLER!!");
    while (1);
}

/* DEBUG_EXC_HANDLER:
 *  description: handle the DEBUG exception
 *  input: none 
 *  output: none
 */
void DEBUG_EXC_HANDLER()
{
    printf("DEBUG_EXC_HANDLER!!");
    while (1);
}

/* NMI_INTERRUPT_HANDLER:
 *  description: handle the NMI_INTERRUPT exception
 *  input: none 
 *  output: none
 */
void NMI_INTERRUPT_HANDLER()
{
    printf("NMI_INTERRUPT_HANDLER!!");
    while (1);
}

/* BREAKPOINT_EXC_HANDLER:
 *  description: handle the BREAKPOINT exception
 *  input: none 
 *  output: none
 */
void BREAKPOINT_EXC_HANDLER()
{
    printf("BREAKPOINT_EXC_HANDLER!!");
    while (1);
}

/* OVERFLOW_EXC_HANDLER:
 *  description: handle the OVERFLOW exception
 *  input: none 
 *  output: none
 */
void OVERFLOW_EXC_HANDLER()
{
    printf("OVERFLOW_EXC_HANDLER!!");
    while (1);
}

/* BOUND_RANGE_EXCEEDED_EXC_HANDLER:
 *  description: handle the BOUND_RANGE_EXCEEDED exception
 *  input: none 
 *  output: none
 */
void BOUND_RANGE_EXCEEDED_EXC_HANDLER()
{
    printf("BOUND_RANGE_EXCEEDED_EXC_HANDLER!!");
    while (1);
}

/* INVALID_OPCODE_EXC_HANDLER:
 *  description: handle the INVALID_OPCODE exception
 *  input: none 
 *  output: none
 */
void INVALID_OPCODE_EXC_HANDLER()
{
    printf("INVALID_OPCODE_EXC_HANDLER!!");
    while (1);
}

/* DEVICE_NOT_AVAILABLE_EXC_HANDLER:
 *  description: handle the DEVICE_NOT_AVAILABLE exception
 *  input: none 
 *  output: none
 */
void DEVICE_NOT_AVAILABLE_EXC_HANDLER()
{
    printf("DEVICE_NOT_AVAILABLE_EXC_HANDLER!!");
    while (1);
}

/* DOUBLE_FAULT_EXC_HANDLER:
 *  description: handle the DOUBLE_FAULT exception
 *  input: none 
 *  output: none
 */
void DOUBLE_FAULT_EXC_HANDLER()
{
    printf("DOUBLE_FAULT_EXC_HANDLER!!");
    while (1);
}

/* COPROCESSOR_SEGMENT_OVERRUN_HANDLER:
 *  description: handle the COPROCESSOR_SEGMENT_OVERRUN exception
 *  input: none 
 *  output: none
 */
void COPROCESSOR_SEGMENT_OVERRUN_HANDLER()
{
    printf("COPROCESSOR_SEGMENT_OVERRUN_HANDLER!!");
    while (1);
}

/* INVALID_TSS_EXC_HANDLER:
 *  description: handle the INVALID_TSS exception
 *  input: none 
 *  output: none
 */
void INVALID_TSS_EXC_HANDLER()
{
    printf("INVALID_TSS_EXC_HANDLER!!");
    while (1);
}

/* SEGMENT_NOT_PRESENT_HANDLER:
 *  description: handle the SEGMENT_NOT_PRESENT exception
 *  input: none 
 *  output: none
 */
void SEGMENT_NOT_PRESENT_HANDLER()
{
    printf("SEGMENT_NOT_PRESENT_HANDLER!!");
    while (1);
}

/* STACK_FAULT_EXCEPTION_HANDLER:
 *  description: handle the STACK_FAULT exception
 *  input: none 
 *  output: none
 */
void STACK_FAULT_EXCEPTION_HANDLER()
{
    printf("STACK_FAULT_EXCEPTION_HANDLER!!");
    while (1);
}

/* GENERAL_PROTECTION_EXC_HANDLER:
 *  description: handle the GENERAL_PROTECTION exception
 *  input: none 
 *  output: none
 */
void GENERAL_PROTECTION_EXC_HANDLER()
{
    printf("GENERAL_PROTECTION_EXC_HANDLER!!");
    while (1);
}

/* PAGE_FAULT_EXC_HANDLER:
 *  description: handle the PAGE_FAULT exception
 *  input: none 
 *  output: none
 */
void PAGE_FAULT_EXC_HANDLER()
{
    printf("PAGE_FAULT_EXC_HANDLER!!");
    while (1);
}

/* PRESERVED_HANDLER:
 *  description: handle the PRESERVED exception
 *  input: none 
 *  output: none
 */
void PRESERVED_HANDLER()
{
    printf("PRESERVED_HANDLER!!");
    while (1);
}

/* X87_FPU_FLOATING_POINT_ERROR_HANDLER:
 *  description: handle the X87_FPU_FLOATING_POINT_ERROR 
 *  input: none 
 *  output: none
 */
void X87_FPU_FLOATING_POINT_ERROR_HANDLER()
{
    printf("X87_FPU_FLOATING_POINT_ERROR_HANDLER!!");
    while (1);
}

/* MACHINE_CHECK_EXC_HANDLER:
 *  description: handle the MACHINE_CHECK exception
 *  input: none 
 *  output: none
 */
void ALIGNMENT_CHECK_EXC_HANDLER()
{
    printf("ALIGNMENT_CHECK_EXC_HANDLER!!");
    while (1);
}

/* MACHINE_CHECK_EXC_HANDLER:
 *  description: handle the MACHINE_CHECK exception
 *  input: none 
 *  output: none
 */
void MACHINE_CHECK_EXC_HANDLER()
{
    printf("MACHINE_CHECK_EXC_HANDLER!!");
    while (1);
}


/* SIMD_FLOATING_POINT_EXC_HANDLER:
 *  description: handle the SIMD_FLOATING_POINT exception
 *  input: none 
 *  output: none
 */
void SIMD_FLOATING_POINT_EXC_HANDLER()
{
    printf("SIMD_FLOATING_POINT_EXC_HANDLER!!");
    while (1);
}
