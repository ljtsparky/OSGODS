// handles the int from keyboards
#include "rtc.h"
#include "lib.h"
#include "i8259.h"

/*
 *rtc_intr_handler
 * description:
 * input: none
 * output: none
 */
void rtc_intr_handler()
{
    cli();
    // test_interrupts();
    /* Make RTC keep receiving interrupts*/
    outb(RTC_STATUS_REG_C, DISABLE_NMI_PORT);
    inb(CMOS_PORT);
    /* rtc will handle another interrupt after send this */
    send_eoi(RTC_IRQ);
    sti();
}

/*
 *init_rtc
 * description: initialize the rtc
 * input: none
 * output: none
 */
void init_rtc() /* initialize the real time clock  */
{
    cli();                                    /* disable other interrupts first, or RTC will be in "undfined" state */
    outb(RTC_STATUS_REG_B, DISABLE_NMI_PORT); /* select register B and disable NMI */
    uint8_t prev = inb(CMOS_PORT);            /* read the value of register B */
    outb(RTC_STATUS_REG_B, DISABLE_NMI_PORT); /* set the index again */
    outb((prev | BIT_6_MASK), CMOS_PORT);     /* write the bit 6 of previous value in register B */

    outb(RTC_STATUS_REG_A, DISABLE_NMI_PORT);
    enable_irq(RTC_IRQ);
    /* set to the frequency of 2Hz */
    outb(FREQUENCY_2_RATE, CMOS_PORT); 
    printf("RTC interrupt enabled \n");
    sti();
}
