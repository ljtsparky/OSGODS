//handles the int from keyboards 
#include "keyboard.h"
#include "lib.h"
#include "i8259.h"

/*
 *keyb_intr_handler
 * description: get the input from keyboard and display it on the monitor
 * input: none
 * output: none
 */
void keyb_intr_handler(){
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
 *init_keyb
 * description: initialize the keyboard
 * input: none
 * output: none
 */
void init_keyb ()
{
    enable_irq(KEYB_IRQ);   /* initialize the keyborad irq */
    printf("keyboard interrupt enabled");
}
