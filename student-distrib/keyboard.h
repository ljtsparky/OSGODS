#ifndef _KEYBORAD_H
#define _KEYBOARD_H

#define KEYB_IRQ 1
#define KEYB_PORT_DATA    0x60
#define KEYB_PORT_COMMAND 0x64
#define KEYB_MAP_LENGTH 59      
#define Q_KEYB_INDEX 16
#define W_KEYB_INDEX 17
#define E_KEYB_INDEX 18
#define R_KEYB_INDEX 19
#define T_KEYB_INDEX 20
#define Y_KEYB_INDEX 21
#define U_KEYB_INDEX 22

#define KEYB_BUFF_LEN 128
#define STATE_NUM     4

#define LEFT_SHIFT_DOWN 0x2A
#define LEFT_SHIFT_UP   0xAA
#define LEFT_CTRL_DOWN  0x1D
#define LEFT_CTRL_UP    0x9D
// #define LEFT_ALT_DOWN   0x38
// #define LEFT_ALT_UP     0xB8
#define CASPLK_CHANGE   0x3A
#define BACKSPACE_DOWN  0x0E
#define DOWN            1
#define UP              0

#define ONLY_SHIFT_DOWN_INDEX   1   /* status index for keyboard map */
#define ONLY_CAPSLK_DOWN_INDEX  2
#define SHIFT_CAPSLK_DOWN_INDEX 3
#define NO_COMMAND_DOWN_INDEX   0

#define KEYB_MAP_L_INDEX    38      /* index of 'L' is keyboard map */
#define KEYB_MAP_TAB_INDEX  15      /* index of '\t' is keyboard map */

void init_keyb();
void keyb_intr_handler();
int read_keyb_signal ();
void set_read_signal_unable();
void set_read_signal_enable();
char* get_keyb_buffer();
void clean_keyb_buffer();
int get_keyb_buffer_index();
#endif
