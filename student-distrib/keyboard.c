//handles the int from keyboards 
#include "keyboard.h"
#include "lib.h"
#include "i8259.h"

/* keyboard map in linux driver */
unsigned char keyboard_map[KEYB_MAP_LENGTH] = {
            0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9',  /* index = 10 */
          '0',   0,   0,   0,   0, 'q', 'w', 'e', 'r', 't',  /* index = 20 */
          'y', 'u', 'i', 'o', 'p',   0,   0,   0,   0, 'a',  /* index = 30 */
          's', 'd', 'f', 'g', 'h', 'j', 'k', 'l',   0,   0,  /* index = 40 */
            0,   0,   0, 'z', 'x', 'c', 'v', 'b', 'n', 'm'   /* index = 50 */
    };
/*
 *keyb_intr_handler
 * description: get the input from keyboard and display it on the monitor
 * input: none
 * output: none
 */
int last_input = -1; /* last index for keyboard map */
void keyb_intr_handler(){
    cli();
    int keyb_input = inb(KEYB_PORT_DATA);    /* current index for keyboard map */
    /* we receive double interrupt for q, w, e, r, t, y, u and mask the second interrupt */
    if (last_input == Q_KEYB_INDEX || last_input == W_KEYB_INDEX || last_input == E_KEYB_INDEX ||
     last_input == R_KEYB_INDEX || last_input == T_KEYB_INDEX || last_input == Y_KEYB_INDEX || last_input == U_KEYB_INDEX) {
        send_eoi(KEYB_IRQ); //send end of interrupt at the end?
        sti();
        last_input = -1;    /* reset the last input index */
    }
    /* in the case we don't receive double interrupt, display the letter or number */
    else {
        /* check whether the keyboard input is number or lowercase letter */
        if ( (keyboard_map[keyb_input] <= NINE_ASCII && keyboard_map[keyb_input] >= ZERO_ASCII) ||
            (keyboard_map[keyb_input] <= z_ASCII && keyboard_map[keyb_input] >= a_ASCII)) {
                putc(keyboard_map[keyb_input]);
            }
        send_eoi(KEYB_IRQ); //send end of interrupt at the end?
        sti();
        last_input = keyb_input;        /* record the last index for keyboard map */
    }
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
    printf("keyboard interrupt enabled \n");
}
