#ifndef _KEYBORAD_H
#define _KEYBOARD_H

#define KEYB_IRQ 1
#define ZERO_ASCII 0x30
#define NINE_ASCII 0X39
#define a_ASCII    0x61
#define z_ASCII    0x7A
#define KEYB_PORT_COMMAND 0x64
#define KEYB_PORT_DATA    0x60





extern void init_keyb();
extern void keyb_intr_handler();

#endif
