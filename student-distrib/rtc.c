//handles the int from keyboards 
#include "rtc.h"
#include "lib.h"
#include "i8259.h"

/*
 *rtc_intr_handler
 * description: 
 * input: none
 * output: none
 */
void rtc_intr_handler(){
    uint32_t rtc_input;    /* value from the keyboard */
    cli();
    
    
    //rtc_input = inb(CMOS_PORT);  /* register C tells a bitmask of what interrput happened */

    test_interrupts();//zhe ge ce shi de buying gai zai zhe  li ba, na zai na li
    //intr handler yao fang zai launch test
    
    // if ( (rtc_input <= NINE_ASCII && rtc_input >= ZERO_ASCII) ||
    //      (rtc_input <= z_ASCII && rtc_input >= a_ASCII)) {
    //         putc(rtc_input);  //putc in keyboard_handler, not here?
    //     }
     
    printf("RTC interrupt can be received!");
    /* Make RTC keep receiving interrupts*/
    outb(RTC_STATUS_REG_C, DISABLE_NMI_PORT);
    inb(CMOS_PORT);
    send_eoi(RTC_IRQ);
    sti();
}

/*
 *init_rtc
 * description: initialize the rtc 
 * input: none
 * output: none
 */
void init_rtc ()
{   
    cli();       /* disable other interrupts first, or RTC will be in "undfined" state */
    outb(RTC_STATUS_REG_B, DISABLE_NMI_PORT);     /* select register B and disable NMI */
    char prev = inb(CMOS_PORT);        /* read the value of register B */
    outb(DISABLE_NMI_PORT, RTC_STATUS_REG_B);    /* set the index again */
    outb((prev | BIT_6_MASK), CMOS_PORT);      /* write the bit 6 of previous value in register B */


    enable_irq(RTC_IRQ);   /* initialize the real time clock  */
    printf("real time clock interrupt enabled");
    sti();
}

