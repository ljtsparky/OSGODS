//handles the int from keyboards 
#include "keyboard.h"
#include "lib.h"
#include "i8259.h"
#include "terminal.h"
#include "execute.h"
#include "page.h"

volatile int copy_signal = 0; /* signal sent to terminal_read function indicating read is allowed */
char key_buffer[KEYB_BUFF_LEN]; /* interrupt keyboard buffer */
int keyb_index; /* index for keyboard buffer */
char buf[KEYB_BUFF_LEN];
/* keyboard map */
char keyboard_map[STATE_NUM][KEYB_MAP_LENGTH] = {
    /* status 1: keyboard map for none function command down */
    {
    '\0','\0',                                          
    '1','2','3','4','5','6','7','8','9','0','-','=',    
    '\b',                                               
    '\t',
    'q','w','e','r','t','y','u','i','o','p','[',']',    
    '\n',                                               
    '\0',                                            
    'a','s','d','f','g','h','j','k','l',';','\'','`',   
    '\0', '\\',                                         
    'z','x','c','v','b','n','m',',','.','/', '\0', '\0',     
    '\0', ' ', '\0'                                    
    },
    /* status 2: keyboard map for only ctrl command down */
    {
    '\0','\0',                                         
    '!','@','#','$','%','^','&','*','(',')','_','+',   
    '\b',                                               
    '\t',                                              
    'Q','W','E','R','T','Y','U','I','O','P','{','}',    
    '\n',                                               
    '\0',                                               
    'A','S','D','F','G','H','J','K','L',':','\"','~',  
    '\0', '|',                                          
    'Z','X','C','V','B','N','M','<','>','?', '\0', '\0',      
    '\0', ' ', '\0'                                   
    },
    /* status 3: keyboard map for only CapsLk command down */
    {
    '\0','\0',                                          
    '1','2','3','4','5','6','7','8','9','0','-','=',    
    '\b',                                               
    '\t',                                               
    'Q','W','E','R','T','Y','U','I','O','P','[',']',    
    '\n',                                               
    '\0',                                               
    'A','S','D','F','G','H','J','K','L',';','\'','`',   
    '\0', '\\',                                         
    'Z','X','C','V','B','N','M',',','.','/', '\0', '\0',   
    '\0', ' ', '\0'                                    
    },
    /* status 4: keyboard map for both ctrl and CapsLk command down */
    {
    '\0','\0',                                         
    '!','@','#','$','%','^','&','*','(',')','_','+',    
    '\b',                                               
    '\t',                                               
    'q','w','e','r','t','y','u','i','o','p','{','}',    
    '\n',                                               
    '\0',                                               
    'a','s','d','f','g','h','j','k','l',':','\"','~',   
    '\0', '|',                                         
    'z','x','c','v','b','n','m','<','>','?', '\0', '\0', 
    '\0', ' ', '\0'                                   
    }
};

/* record the status of ctrl, shift, alt, capslk command */
typedef struct command_status
{
    int left_shift; 
    int left_ctrl;
    int left_alt;
    int capslk;
    int alt;
} command_status;

command_status status; /* current status of function command */


/*
 *init_keyb
 * description: initialize the keyboard
 * input: none
 * output: none
 */
void init_keyb ()
{
    enable_irq(KEYB_IRQ);   /* initialize the keyborad irq */
    status.capslk = UP;
    status.left_shift = UP;
    status.left_ctrl = UP;
    status.left_alt = UP;
    status.alt = UP;
    printf("keyboard interrupt enabled \n");
}

/*
 *keyb_intr_handler
 * description: handle the keyboard interrupt
 * inputs: none
 * outputs: none
 */
int prev_display_tid = 0;
void keyb_intr_handler ()
{
    cli();
    int keyb_input = inb(KEYB_PORT_DATA); /* keyboard map index */
    
    /* handle the function command status change */
    switch (keyb_input)
    {
    case LEFT_SHIFT_DOWN:
        status.left_shift = DOWN;
        goto end;

    case LEFT_SHIFT_UP:
        status.left_shift = UP;
        goto end;

    case LEFT_CTRL_DOWN:
        status.left_ctrl= DOWN;
        goto end;

    case LEFT_CTRL_UP:
        status.left_ctrl= UP;
        goto end;

    case LEFT_ALT_DOWN:
        status.alt = DOWN;
        goto end;
    
    case LEFT_ALT_UP:
        status.alt = UP;
        goto end;

    case CASPLK_CHANGE:
        if (status.capslk == DOWN) {
            status.capslk = UP;
        } else {
            status.capslk = DOWN;
        }
        goto end;
    
    default:
        break;
    }

    int display_tid = get_display_tid();
    update_video_mem((int8_t*)VIDEO_MEMORY_ADDR); /* update video address in putc */
    update_screen_x_y(terminal_list[display_tid].x_cursor, terminal_list[display_tid].y_cursor); /* update the cursor */
    
    /* handle the case when ctrl and l is down */
    if (status.left_ctrl == DOWN && keyb_input == KEYB_MAP_L_INDEX ) {
        clear();
        goto end;
    }

    if (status.alt == DOWN && keyb_input == F1_DOWN) {
        terminal_switch(0);
        // printf("F1 DOWN    ");
        goto end;
    }

    if (status.alt == DOWN && keyb_input == F2_DOWN) {
        terminal_switch(1);
        // printf("F2 DOWN    ");    
        goto end;
    }

    if (status.alt == DOWN && keyb_input == F3_DOWN) {
        terminal_switch(2);
        // printf("F3 DOWN    ");
        goto end;
    }

    if (keyb_input < KEYB_MAP_LENGTH) {
        char display_char;  /* char to be displayed */
        /* get char when ctrl down, casplk up */
        if (status.left_shift == DOWN && status.capslk == UP) {
            display_char = keyboard_map[ONLY_SHIFT_DOWN_INDEX][keyb_input];
        } 
        /* get char when ctrl up, casplk down */
        else if (status.capslk == DOWN && status.left_shift == UP) {
            display_char = keyboard_map[ONLY_CAPSLK_DOWN_INDEX][keyb_input];
        }
        /* get char when ctrl down, casplk down */
        else if (status.capslk == DOWN && status.left_shift == DOWN) {
            display_char = keyboard_map[SHIFT_CAPSLK_DOWN_INDEX][keyb_input];
           
        }
        /* get char when ctrl up, casplk up */
        else if (status.capslk == UP && status.left_shift == UP) {
            display_char = keyboard_map[NO_COMMAND_DOWN_INDEX][keyb_input];
        }

        /* delete the memory of keyboard buffer when backspace is pressed */
        if (display_char == '\b') {
            if (keyb_index == 0) {  /* if no keyboard interrrupt no need to delete */
                // putc('\b');
                goto end;
            }
            --keyb_index;  
            set_keyb_pressed(); 
            putc('\b');
            goto end;
        }
        
        /* store the keyboard buffer */
        set_keyb_pressed();
        key_buffer[keyb_index++] = display_char; 
        putc(display_char);     /* display the character */
        if (display_char == '\n') {
            set_signal_enable();
        }
    }
/* return cancel the critical section and send EOI*/
end: 
    sti();
    int scheduled_tid = get_scheduled_tid();
    display_tid = get_display_tid();
    if (scheduled_tid != display_tid) {
        update_video_mem(terminal_list[scheduled_tid].video_page);
    } else {
        update_video_mem((int8_t*)VIDEO_MEMORY_ADDR);
    }
    send_eoi(KEYB_IRQ);
}

/* read_keyb_signal 
 * description: read the copy_signal which indicates whether terminal can read the keyb_buff
 * inputs: none
 * outputs: copy_signal 
 */
int read_keyb_signal ()
{
    return copy_signal;
}

/* set_read_signal_unable 
 * description: set copy_signal to 0
 * inputs: none
 * outputs: none
 */
void set_read_signal_unable()
{
    copy_signal = 0;
}

/* set_read_signal_enable 
 * description: set copy_signal to 1
 * inputs: none
 * outputs: none
 */
void set_read_signal_enable()
{
    copy_signal = 1;
}

/* get_keyb_buffer 
 * description: get address of keyboard buffer 
 * inputs: none
 * outputs: address of keyboard buffer one
 */
char* get_keyb_buffer()
{
    return key_buffer;
}

/* get_keyb_buffer_index 
 * description: get index of keyboard buffer 
 * inputs: none
 * outputs: index of keyboard buffer one
 */
int get_keyb_buffer_index(){
    return keyb_index;
}

/* update_keyb_buf
 * description: update index of keyboard buffer 
 * inputs: new keyboard buffer index 
 * outputs: none
 */
void update_keyb_buf(char* new_keyb_buf, int new_index){
    int i;
    cli();
    keyb_index = new_index;
    if (new_index == 0) {
        clean_keyb_buffer();
        sti();
        return;
    } 
    for (i=0; i<new_index; i++) {
        key_buffer[i] = new_keyb_buf[i];
    }
    sti();  
}

/*
 *clean_keyb_buffer
 * description: clean the memory of keyboard buffer
 * input: none
 * output: none
 */
void clean_keyb_buffer()
{
    int i;
    for (i=0; i<keyb_index; i++) {
        key_buffer[i] = 0;
    }
    keyb_index = 0;
}
