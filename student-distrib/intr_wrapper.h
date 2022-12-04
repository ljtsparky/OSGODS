
#ifndef _INTR_WRAPPER_H_
#define _INTR_WRAPPER_H_

#ifndef ASM
    extern void pit_wrapper();
    extern void keyb_wrapper();
    extern void rtc_wrapper();
#endif

#endif
