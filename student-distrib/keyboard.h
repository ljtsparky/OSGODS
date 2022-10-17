#ifndef _KEYBORAD_H
#define _KEYBOARD_H

#define KEYB_IRQ 1
#define ZERO_ASCII 0x30
#define NINE_ASCII 0X39
#define a_ASCII    0x61
#define z_ASCII    0x7A
#define KEYB_PORT_DATA    0x60
#define KEYB_MAP_LENGTH 51
#define Q_KEYB_INDEX 16
#define W_KEYB_INDEX 17
#define E_KEYB_INDEX 18
#define R_KEYB_INDEX 19
#define T_KEYB_INDEX 20
#define Y_KEYB_INDEX 21
#define U_KEYB_INDEX 22


extern void init_keyb();
extern void keyb_intr_handler();

#endif
