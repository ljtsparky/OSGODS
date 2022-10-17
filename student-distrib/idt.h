// CREATED BY li jiatong on 2022 oct 16
#ifndef _IDT_H
#define _IDT_H
#include "x86_desc.h"

extern void init_idt();

enum IDT_VECTORS_FIRST_20
{
    DIVIDE_ERROR_EXC, // 0
    DEBUG_EXC,
    NMI_INTERRUPT,
    BREAKPOINT_EXC,
    OVERFLOW_EXC,
    BOUND_RANGE_EXCEEDED_EXC, // 5
    INVALID_OPCODE_EXC,
    DEVICE_NOT_AVAILABLE_EXC,
    DOUBLE_FAULT_EXC,
    COPROCESSOR_SEGMENT_OVERRUN,
    INVALID_TSS_EXC, // 10
    SEGMENT_NOT_PRESENT,
    STACK_FAULT_EXCEPTION,
    GENERAL_PROTECTION_EXC,
    PAGE_FAULT_EXC,
    PRESERVED, // 15 is preserved in intel manual
    X87_FPU_FLOATING_POINT_ERROR,
    ALIGNMENT_CHECK_EXC,
    MACHINE_CHECK_EXC,
    SIMD_FLOATING_POINT_EXC // 19
};

#define NUM_EXCEPTION 20
#define INT_INDEX     0x20
#define PERSERVED_EXCEPTION 15
#define KEYB_INTERRUPT_INDEX 0x21
#define RTC_INTERRUPT_INDEX 0x28
// extern void exception_handler(uint8_t i);

extern void DIVIDE_ERROR_EXC_HANDLER();
extern void DEBUG_EXC_HANDLER();
extern void NMI_INTERRUPT_HANDLER();
extern void BREAKPOINT_EXC_HANDLER();
extern void OVERFLOW_EXC_HANDLER();
extern void BOUND_RANGE_EXCEEDED_EXC_HANDLER();
extern void INVALID_OPCODE_EXC_HANDLER();
extern void DEVICE_NOT_AVAILABLE_EXC_HANDLER();
extern void DOUBLE_FAULT_EXC_HANDLER();
extern void COPROCESSOR_SEGMENT_OVERRUN_HANDLER();
extern void INVALID_TSS_EXC_HANDLER();
extern void SEGMENT_NOT_PRESENT_HANDLER();
extern void STACK_FAULT_EXCEPTION_HANDLER();
extern void GENERAL_PROTECTION_EXC_HANDLER();
extern void PAGE_FAULT_EXC_HANDLER();
extern void PRESERVED_HANDLER();
extern void X87_FPU_FLOATING_POINT_ERROR_HANDLER();
extern void ALIGNMENT_CHECK_EXC_HANDLER();
extern void MACHINE_CHECK_EXC_HANDLER();
extern void SIMD_FLOATING_POINT_EXC_HANDLER();

#endif