#ifndef _RTC_H
#define _RTC_H

#define RTC_IRQ      8
#define ZERO_ASCII 0x30
#define NINE_ASCII 0X39
#define a_ASCII    0x61
#define z_ASCII    0x7A







extern void init_rtc();
extern void rtc_intr_handler();

#endif