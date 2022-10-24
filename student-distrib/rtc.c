#include "rtc.h"
#include "lib.h"
#include "i8259.h"

volatile int rtc_interrupt_occurred;    //flag to sychronize between rtc_read and rtc_intr_handler
int rtc_interrupt_counter;      //counter to test the change of frequency
/*
 *rtc_intr_handler
 * description: handle each interrupt received by RTC, set flag and add counter
 * input: none
 * output: none
 */
int count=0;
int frequency[10] = {2, 4, 8, 16, 32, 64, 128, 256, 512, 1024 };
void rtc_intr_handler()
{
    cli();
    // rtc_write(0,&frequency[  ((count++)/20) % 9],4);
    // putc('@');
    
    rtc_interrupt_occurred = 1;//when an interrupt occurs, set the flag and add the counter
    rtc_interrupt_counter++;
    /* Make RTC keep receiving interrupts*/
    outb(RTC_STATUS_REG_C, RTC_REG_PORT);
    inb(CMOS_PORT);
    /* rtc will handle another interrupt after send this */
    send_eoi(RTC_IRQ);
    sti();
}

/*
 *init_rtc
 * description: initialize the rtc
 * input: none
 * output: none
 */
void init_rtc() /* initialize the real time clock  */
{
    cli();                                    /* disable other interrupts first, or RTC will be in "undfined" state */
    outb(RTC_STATUS_REG_B, RTC_REG_PORT); /* select register B and disable NMI */
    uint8_t prev = inb(CMOS_PORT);            /* read the value of register B */
    outb(RTC_STATUS_REG_B, RTC_REG_PORT); /* set the index again */
    outb((prev | BIT_6_MASK), CMOS_PORT);     /* write the bit 6 of previous value in register B */

    outb(RTC_STATUS_REG_A, RTC_REG_PORT);
    enable_irq(RTC_IRQ);
    /* set to the frequency of 2Hz */
    outb(FREQUENCY_2_RATE, CMOS_PORT); 
    rtc_interrupt_counter = 0;
    rtc_interrupt_occurred = 0;
    printf("RTC interrupt enabled \n");
    sti();
}

/*
 * rtc_open (const uint8_t* filename)
 * input: ignore
 * output: 0 for success
 * effect: reset RTC frequency to 2Hz
 * */

int32_t rtc_open (const uint8_t* filename)
{
    /* initialize RTC frequency to 2Hz */
    outb(RTC_STATUS_REG_A, RTC_REG_PORT);
    outb(FREQUENCY_2_RATE, CMOS_PORT);
    return 0;
}

/*
 * rtc_close (int32_t fd)
 * input: ignore
 * output: 0 for success
 * effect: none
 * */
int32_t rtc_close (int32_t fd)
{
    return 0;
}


/*
 * rtc_read (int32_t fd, void* buf, int32_t nbytes)
 * input: ignore
 * output: always 0
 * effect: each time a interrupt occurs, return 0
 * */
int32_t rtc_read (int32_t fd, void* buf, int32_t nbytes)
{
    while (rtc_interrupt_occurred != 1){}      //when an interrupt doesn't occur, just wait
        
   
    rtc_interrupt_occurred = 0;              //reset the flag after an interrupt
    return 0;
}


/*
 * rtc_write (int32_t fd, const void* buf, int32_t nbytes)
 * input: fd = file descriptor, buf = pointer to the buffer which stores the frequency in Hz, nbytes = the number of 
 *        bytes of the integer
 * output: 4 for success, -1 for failure
 * effect: change the frequency of the RTC
 * */
int32_t rtc_write (int32_t fd, const void* buf, int32_t nbytes)
{
    /* check for the fail condition */
    if (nbytes != 4){                 //the integer must be 4 bytes
        return -1;
    }

    if (buf == NULL){                 //the pointer can't be NULL
        return -1;
    }

    int32_t rtc_frequency;
    rtc_frequency = *((int32_t*)buf);   //get the frequency to write
    if ((rtc_frequency < MIN_FREQ) || (rtc_frequency > MAX_FREQ)){         //test of in the range of normal frequency
        return -1;
    }

    if ((rtc_frequency & (rtc_frequency - 1)) != 0){    //check for power of 2
    //if ((rtc_frequency & (!rtc_frequency)) != 0){       
        return -1;
    }
    
    uint8_t rate;
    int i;
    for (i = 0; i < 15; i++){                     //rate can be 0-15
        if ((rtc_frequency << i) == 32768){       //frequency = 32768>>(rate-1), formula from https://wiki.osdev.org/RTC
            rate = i + 1;
        }
    }
    rate &= 0x0F;
    cli();
    outb(RTC_STATUS_REG_A, RTC_REG_PORT);         //change to new frequency
    outb(rate, CMOS_PORT);
    sti();
    return 4;                                     //write 4 bytes
}
