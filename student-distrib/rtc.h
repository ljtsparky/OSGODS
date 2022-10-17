#ifndef _RTC_H
#define _RTC_H

#define RTC_IRQ 8
#define ZERO_ASCII 0x30
#define NINE_ASCII 0X39
#define a_ASCII 0x61
#define z_ASCII 0x7A
#define DISABLE_NMI_PORT 0x70 // port to disable NMI
#define RTC_STATUS_REG_A 0x8A
#define RTC_STATUS_REG_B 0x8B
#define RTC_STATUS_REG_C 0x0C
#define CMOS_PORT 0x71
#define BIT_6_MASK 0x40
#define FREQUENCY_1024_RATE 0x06
#define FREQUENCY_2_RATE 0xF

extern void init_rtc();
extern void rtc_intr_handler();

#endif
