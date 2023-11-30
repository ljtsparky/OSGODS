#ifndef _PIT_H
#define _PIT_H // Programmable Interval Timer
//read write 0x40
#define PIT_IO_PORT 0x40 // data port
//read only 0x43
#define PIT_COMMAND_PORT 0x43 // used to indicate different output mode 
#define PIT_MODE_3_BINARY_MODE 0x37  // 00 11 011 0 Mode 3 is square wave generator.
#define PIT_20_HZ 59659 //Constant for setting the PIT to a 20 Hz frequency.
#define EIGHT_BIT_OFFSET 8 
#define PIT_IRQ 0 // The IRQ line that the PIT uses (typically IRQ0).
#define IN_FREQUENCY 11932  // Input frequency to the PIT for division.
void init_pit(void);
void pit_intr_handler(void);
#endif

