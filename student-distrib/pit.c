#include "pit.h"
#include "lib.h"
#include "i8259.h"
#include "types.h"
#include "execute.h"
// int task_switch_flag=0;

/* init_pit
 * input: none
 * output: none
 * side effect: initialize the pit
 */
void init_pit()
{
    // cli();
    outb( PIT_MODE_3_BINARY_MODE, PIT_COMMAND_PORT);
    outb((uint8_t)(IN_FREQUENCY) && 0xff, PIT_IO_PORT); //PUT IN LOW 8 BITS
    outb( (uint8_t)( IN_FREQUENCY >> 8 ), PIT_IO_PORT); //TRANSFER HIGH 8 BITS
    enable_irq(PIT_IRQ); //0 is the pit irq number
    // sti();
}

/* pit_intr_handler
 * input : none
 * output: none
 * side effect: handle the interrupt 
 */
void pit_intr_handler()
{
    send_eoi(PIT_IRQ); // Notify the PIC (Programmable Interrupt Controller) that the PIT interrupt has been handled.
    memory_switch();
    scheduler();   
}

