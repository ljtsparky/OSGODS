//handles the int from keyboards 
#include "rtc.h"
#include "lib.h"
#include "i8259.h"

/*
 *rtc_intr_handler
 * description: get the input from keyboard and display it on the monitor
 * input: none
 * output: none
 */
void rtc_intr_handler(){
    uint32_t keyb_input;    /* value from the keyboard */

    cli();
    keyb_input = inb();
    /* check whether the keyboard input is number or lowercase letter */
    if ( (keyb_input <= NINE_ASCII && keyb_input >= ZERO_ASCII) ||
         (keyb_input <= z_ASCII && keyb_input >= a_ASCII)) {
            putc(keyb_input);
        }
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
    enable_irq(RTC_IRQ);   /* initialize the real time clock  */
    printf("keyboard interrupt enabled");
}
