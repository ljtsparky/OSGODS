#include "idt.h"
#include "lib.h"
#include "keyboard.h"
#include "rtc.h"
/* exception handler function table */
void *exception_handler_table[20] = { // we use this table to locate the function address of these handlers
    DIVIDE_ERROR_EXC_HANDLER,
    DEBUG_EXC_HANDLER,
    NMI_INTERRUPT_HANDLER,
    BREAKPOINT_EXC_HANDLER,
    OVERFLOW_EXC_HANDLER,
    BOUND_RANGE_EXCEEDED_EXC_HANDLER,
    INVALID_OPCODE_EXC_HANDLER,
    DEVICE_NOT_AVAILABLE_EXC_HANDLER,
    DOUBLE_FAULT_EXC_HANDLER,
    COPROCESSOR_SEGMENT_OVERRUN_HANDLER,
    INVALID_TSS_EXC_HANDLER,
    SEGMENT_NOT_PRESENT_HANDLER,
    STACK_FAULT_EXCEPTION_HANDLER,
    GENERAL_PROTECTION_EXC_HANDLER,
    PAGE_FAULT_EXC_HANDLER,
    PRESERVED_HANDLER,
    X87_FPU_FLOATING_POINT_ERROR_HANDLER,
    ALIGNMENT_CHECK_EXC_HANDLER,
    MACHINE_CHECK_EXC_HANDLER,
    SIMD_FLOATING_POINT_EXC_HANDLER};

void init_idt()
{
    int i;
    for (i = 0; i < NUM_VEC; i++)
    {
        idt[i].seg_selector = KERNEL_CS;
        idt[i].reserved4 = 0;
        idt[i].size = 1;
        idt[i].reserved3 = 1;
        idt[i].reserved2 = 1;
        idt[i].reserved1 = 1;
        idt[i].reserved0 = 0;
        idt[i].dpl = 0;
        idt[i].present = 1;
    }

    /* fill the exception */
    for (i = 0; i < NUM_EXCEPTION; i++)
    {
        SET_IDT_ENTRY(idt[i], exception_handler_table[i]);
    }

    /* fill the unuesed exception */
    for (i = NUM_EXCEPTION; i < INT_INDEX; i++)
    {
        SET_IDT_ENTRY(idt[i], exception_handler_table[PERSERVED_EXCEPTION]);
    }

    SET_IDT_ENTRY(idt[KEYB_INTERRUPT_INDEX], keyb_intr_handler); // keboard interrupt
    idt[KEYB_INTERRUPT_INDEX].reserved3 = 0;
    SET_IDT_ENTRY(idt[RTC_INTERRUPT_INDEX], rtc_intr_handler); // rtc interrupt
    idt[RTC_INTERRUPT_INDEX].reserved3 = 0;
}

/* DIVIDE_ERROR_EXC_HANDLER:
 *  description: handle the DIVIDE_ERROR exception
 *  input: none
 *  output: none
 */
void DIVIDE_ERROR_EXC_HANDLER()
{
    printf("DIVIDE_ERROR_EXC_HANDLER!!\n");
    while (1)
        ;
}

/* DEBUG_EXC_HANDLER:
 *  description: handle the DEBUG exception
 *  input: none
 *  output: none
 */
void DEBUG_EXC_HANDLER()
{
    printf("DEBUG_EXC_HANDLER!!\n");
    while (1)
        ;
}

/* NMI_INTERRUPT_HANDLER:
 *  description: handle the NMI_INTERRUPT exception
 *  input: none
 *  output: none
 */
void NMI_INTERRUPT_HANDLER()
{
    printf("NMI_INTERRUPT_HANDLER!!\n");
    while (1)
        ;
}

/* BREAKPOINT_EXC_HANDLER:
 *  description: handle the BREAKPOINT exception
 *  input: none
 *  output: none
 */
void BREAKPOINT_EXC_HANDLER()
{
    printf("BREAKPOINT_EXC_HANDLER!!\n");
    while (1)
        ;
}

/* OVERFLOW_EXC_HANDLER:
 *  description: handle the OVERFLOW exception
 *  input: none
 *  output: none
 */
void OVERFLOW_EXC_HANDLER()
{
    printf("OVERFLOW_EXC_HANDLER!!\n");
    while (1)
        ;
}

/* BOUND_RANGE_EXCEEDED_EXC_HANDLER:
 *  description: handle the BOUND_RANGE_EXCEEDED exception
 *  input: none
 *  output: none
 */
void BOUND_RANGE_EXCEEDED_EXC_HANDLER()
{
    printf("BOUND_RANGE_EXCEEDED_EXC_HANDLER!!\n");
    while (1)
        ;
}

/* INVALID_OPCODE_EXC_HANDLER:
 *  description: handle the INVALID_OPCODE exception
 *  input: none
 *  output: none
 */
void INVALID_OPCODE_EXC_HANDLER()
{
    printf("INVALID_OPCODE_EXC_HANDLER!!\n");
    while (1)
        ;
}

/* DEVICE_NOT_AVAILABLE_EXC_HANDLER:
 *  description: handle the DEVICE_NOT_AVAILABLE exception
 *  input: none
 *  output: none
 */
void DEVICE_NOT_AVAILABLE_EXC_HANDLER()
{
    printf("DEVICE_NOT_AVAILABLE_EXC_HANDLER!!\n");
    while (1)
        ;
}

/* DOUBLE_FAULT_EXC_HANDLER:
 *  description: handle the DOUBLE_FAULT exception
 *  input: none
 *  output: none
 */
void DOUBLE_FAULT_EXC_HANDLER()
{
    printf("DOUBLE_FAULT_EXC_HANDLER!!\n");
    while (1)
        ;
}

/* COPROCESSOR_SEGMENT_OVERRUN_HANDLER:
 *  description: handle the COPROCESSOR_SEGMENT_OVERRUN exception
 *  input: none
 *  output: none
 */
void COPROCESSOR_SEGMENT_OVERRUN_HANDLER()
{
    printf("COPROCESSOR_SEGMENT_OVERRUN_HANDLER!!\n");
    while (1)
        ;
}

/* INVALID_TSS_EXC_HANDLER:
 *  description: handle the INVALID_TSS exception
 *  input: none
 *  output: none
 */
void INVALID_TSS_EXC_HANDLER()
{
    printf("INVALID_TSS_EXC_HANDLER!!\n");
    while (1)
        ;
}

/* SEGMENT_NOT_PRESENT_HANDLER:
 *  description: handle the SEGMENT_NOT_PRESENT exception
 *  input: none
 *  output: none
 */
void SEGMENT_NOT_PRESENT_HANDLER()
{
    printf("SEGMENT_NOT_PRESENT_HANDLER!!\n");
    while (1)
        ;
}

/* STACK_FAULT_EXCEPTION_HANDLER:
 *  description: handle the STACK_FAULT exception
 *  input: none
 *  output: none
 */
void STACK_FAULT_EXCEPTION_HANDLER()
{
    printf("STACK_FAULT_EXCEPTION_HANDLER!!\n");
    while (1)
        ;
}

/* GENERAL_PROTECTION_EXC_HANDLER:
 *  description: handle the GENERAL_PROTECTION exception
 *  input: none
 *  output: none
 */
void GENERAL_PROTECTION_EXC_HANDLER()
{
    printf("GENERAL_PROTECTION_EXC_HANDLER!!\n");
    while (1)
        ;
}

/* PAGE_FAULT_EXC_HANDLER:
 *  description: handle the PAGE_FAULT exception
 *  input: none
 *  output: none
 */
void PAGE_FAULT_EXC_HANDLER()
{
    printf("PAGE_FAULT_EXC_HANDLER!!\n");
    while (1)
        ;
}

/* PRESERVED_HANDLER:
 *  description: handle the PRESERVED exception
 *  input: none
 *  output: none
 */
void PRESERVED_HANDLER()
{
    printf("PRESERVED_HANDLER!!\n");
    while (1)
        ;
}

/* X87_FPU_FLOATING_POINT_ERROR_HANDLER:
 *  description: handle the X87_FPU_FLOATING_POINT_ERROR
 *  input: none
 *  output: none
 */
void X87_FPU_FLOATING_POINT_ERROR_HANDLER()
{
    printf("X87_FPU_FLOATING_POINT_ERROR_HANDLER!!\n");
    while (1)
        ;
}

/* MACHINE_CHECK_EXC_HANDLER:
 *  description: handle the MACHINE_CHECK exception
 *  input: none
 *  output: none
 */
void ALIGNMENT_CHECK_EXC_HANDLER()
{
    printf("ALIGNMENT_CHECK_EXC_HANDLER!!\n");
    while (1)
        ;
}

/* MACHINE_CHECK_EXC_HANDLER:
 *  description: handle the MACHINE_CHECK exception
 *  input: none
 *  output: none
 */
void MACHINE_CHECK_EXC_HANDLER()
{
    printf("MACHINE_CHECK_EXC_HANDLER!!\n");
    while (1)
        ;
}

/* SIMD_FLOATING_POINT_EXC_HANDLER:
 *  description: handle the SIMD_FLOATING_POINT exception
 *  input: none
 *  output: none
 */
void SIMD_FLOATING_POINT_EXC_HANDLER()
{
    printf("SIMD_FLOATING_POINT_EXC_HANDLER!!\n");
    while (1)
        ;
}
