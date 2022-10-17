/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void i8259_init(void)
{
    // printf("try initialize i8259\n");
    unsigned long flags; /* save the current IF */
    cli_and_save(flags); /* create the critical section */

    outb(BYTE_MASK, MASTER_8259_PORT_1); /* mask all 8259A-1 */
    outb(BYTE_MASK, SLAVE_8259_PORT_1);  /* mask all 8259A-2 */

    outb(ICW1, MASTER_8259_PORT_0);            /* ICW1: set 8259A-1 init */
    outb(ICW2_MASTER + 0, MASTER_8259_PORT_1); /* ICW2: 8259A-1 IR0-7 mapped to 0x20-0x27      */
    outb(ICW3_MASTER, MASTER_8259_PORT_1);     /* 8259A-1 (the primary)has a secondary on IRQ2 */

    outb(ICW1, SLAVE_8259_PORT_0);       /* ICW1: set 8259A-2 init */
    outb(ICW2_SLAVE, SLAVE_8259_PORT_1); /* ICW2: 8259A-1 IR0-7 mapped to 0x28-0x2f      */
    outb(ICW3_SLAVE, SLAVE_8259_PORT_1); /* 8259A-2 is a secondary on primary’s IRQ2     */
    outb(ICW4, SLAVE_8259_PORT_1);

    /* diable all the irq except the cascade to secondary irq in master pic */
    int i; /* index for irq */
    for (i = 0; i < TOTAL_IRQ; i++)
    {
        if (i == CASCADE_TO_SECONDARY)
        {
            enable_irq(CASCADE_TO_SECONDARY); /* enable the irq 2 in master pic */
        }
        else
        {
            disable_irq(i);
        }
    }

    sti();
    restore_flags(flags); /* restore the IF */
    printf("initialize i8259 finished\n");
}

/* enable_irq
 *  description: enable the specific irq
 *  input: irq number
 *  output: none
 */
void enable_irq(uint32_t irq_num)
{
    if (irq_num >= NUM_MASTER_IRQ)
    {
        slave_mask &= ~(BIT_MASK << (irq_num - NUM_MASTER_IRQ)); /* set the irq bit to zero */
        outb(slave_mask, SLAVE_8259_PORT_1);
    }
    else
    {
        master_mask &= ~(BIT_MASK << (irq_num)); /* set the irq bit to zero */
        outb(master_mask, MASTER_8259_PORT_1);
    }
}

/* disable_irq
 *  description: disable the specific irq
 *  input: irq number
 *  output: none
 */
void disable_irq(uint32_t irq_num)
{
    /* check the irq is in slave or not */
    if (irq_num >= NUM_MASTER_IRQ)
    {
        slave_mask |= (BIT_MASK << (irq_num - NUM_MASTER_IRQ)); /* set the irq bit to one */
        outb(slave_mask, SLAVE_8259_PORT_1);
    }
    else
    {
        master_mask |= (BIT_MASK << (irq_num)); /* set the irq bit to one */
        outb(master_mask, MASTER_8259_PORT_1);
    }
}

/* send_eoi
 *  description: send end-of-interrupt signal for the specified IRQ
 *  input: irq number
 *  output: none
 */
void send_eoi(uint32_t irq_num)
{
    /* check the irq is in slave or not */
    if (irq_num >= NUM_MASTER_IRQ)
    {
        outb(EOI | (irq_num - NUM_MASTER_IRQ), SLAVE_8259_PORT_0); /* send the eoi to slave IRQ */
        outb(EOI | CASCADE_TO_SECONDARY, MASTER_8259_PORT_0);      /* send the eoi to master IRQ */
    }
    else
    {
        outb(EOI | (irq_num), MASTER_8259_PORT_0); /* send the eoi to master IRQ */
    }
}
