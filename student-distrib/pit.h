#ifndef _PIT_H
#define _PIT_H
//read write 0x40
#define PIT_IO_PORT 0x40 // data port
//read only 0x43
#define PIT_COMMAND_PORT 0x43 // used to indicate different output mode 
#define PIT_MODE_3_BINARY_MODE 0x37  // 00 11 011 0
#define PIT_20_HZ 59659
#define EIGHT_BIT_OFFSET 8
#define PIT_IRQ 0
#define IN_FREQUENCY 11932
void init_pit(void);
void pit_intr_handler(void);
#endif

