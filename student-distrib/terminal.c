#include "lib.h"
#include "keyboard.h"
#include "terminal.h"
#include "page.h"
#include "types.h"
#include "execute.h"
#include "x86_desc.h"

int num_bytes_to_copy = 0;
int terminal_buf_index;
int curr_terminal_num = 0;
int scheduled_terminal_num = 0;
#define NUM_ROW 25 
#define NUM_COL 80
#define BYTE 4

//terminal_bad_read
//input: file descriptor, buffer, number of bytes
//ouput:-1 for the bad read
int terminal_bad_read(int32_t fd, void* buf ,int nbytes){
    printf("terminal bad read!!\n");
    return -1;
}

//terminal_bad_write
//input: file descriptor, buffer, number of bytes
//ouput:-1 for the bad write
int terminal_bad_write(int32_t fd, const void* buf ,int nbytes){
    printf("terminal bad write!!\n");
    return -1;
}

//get_terminal_buf
//input: none
//output:none
//return : return the pointer to the 128 char array terminal_buf
//side effect: none
terminal_t* get_terminal_buf()
{
    return &terminal_list[0];
}

//terminal_open
//by opening, we want the terminal buffer set to all zero inside and make the index back to 0
//input: none   output: terminal opened msg   return: 0 for success   no side-effect
int32_t terminal_open(const uint8_t* name){
    return 0;
} 

//terminal_close
//clear the buffer, set 0 of index count
//input: none    output: none    return: 0 for success    no side-effect
int32_t terminal_close(int32_t fd){
    terminal_buf_index=0; //reset index to zero
    return 0;
}

volatile int terminal_read_enable_signal = 0;

//set_signal_enable
//input: none
//ouput:none
//side effect: enbale the signal for terminal read
void set_signal_enable()
{
    terminal_read_enable_signal = 1;
}
//terminal_read
//description: Copy from keyboard handler's kb_buff to function argument buf
//input: file descriptor, buffer pointer to be copied into, nbytes: how many bytes to write into buf
//output: if normal, print message, if not , print wrong case message
//return: how many bytes are successfully read     no side-effect
int32_t terminal_read(int32_t fd, void* buf ,int32_t nbytes){   
     
    // printf("in terminal read 1\n");
    // while();
    while(!terminal_read_enable_signal || scheduled_terminal_num!=get_display_tid());
    // while(!terminal_list[get_display_terminal_num()].terminal_read_enabled);
    terminal_read_enable_signal = 0;
    // terminal_list[get_display_terminal_num()].terminal_read_enabled=0;
    if (get_keyb_buffer_index() >= KEYB_BUFFER_SIZE){ //make sure the bytes we read is no bigger than max buffer size
        printf("too much to read!!\n");
        clean_keyb_buffer();
        return 0;
    }

    char* keyb_buf = get_keyb_buffer();//get the keyboard buffer pointer
    char* buf1 = (char*) buf;
    int keyb_index = get_keyb_buffer_index();
    int i;
    for (i=0; i < keyb_index; i++) { //i is the iterator and counts how many bytes are written
        buf1[i] = keyb_buf[i];
    }
    buf1[get_keyb_buffer_index()] = '\0';
    clean_keyb_buffer();//clean the keyboard buffer since it is already read
    return i;
}

//terminal_write
//description: put a buffer on screen
//input: buffer pointer, and num_of_bytes to be put
//output: the characters in the buffer, from the start to the num of bytes
//return : number of bytes that is displayed.
//side effect: none
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes){
    
    int i;
    char* buf1 = (char*)buf;
    cli();
    // cli_and_save(flags);
    for (i=0; i<nbytes; i++) { //go through the buffer pointer first nbytes
        putc(buf1[i]);
    }
    // restore_flags(flags);
    sti();
    return i;
}

//terminal_init
//description:like the open, we initialize the buffer content and index of buffer to 0
//input : none   
//output:none  
//return : none  
//side effect:no side-effect
void terminal_init()
{
    int i;
    char keyb_buffer[MAX_TERMINAL_NUM][KEYB_BUFFER_SIZE];
    for (i=0; i<MAX_TERMINAL_NUM; i++) {
        terminal_list[i].tid = i;
        terminal_list[i].x_cursor = 0;
        terminal_list[i].y_cursor = 0;
        memset(keyb_buffer[i], 0, KEYB_BUFFER_SIZE);
        memset(terminal_list[i].prev_screen_x, 0, BYTE*NUM_ROW);
        memset(terminal_list[i].line_is_full, 0, BYTE*NUM_ROW);
        memset(terminal_list[i].display_buf,0,BYTE*NUM_ROW*NUM_COL);
        terminal_list[i].i_prev_x = 0;
        terminal_list[i].display_buf_index = 0;
        terminal_list[i].keyb_buffer = keyb_buffer[i];
        terminal_list[i].keyb_buffer_index = 0;
        terminal_list[i].next_terminal = (void*)&terminal_list[(i + 1) % MAX_TERMINAL_NUM];
        terminal_list[i].pcb_ptr = NULL;
        terminal_list[i].count_pcb = 0;
        terminal_list[i].video_page = (int8_t*)(VIDEO_MEMORY_ADDR + (i + 1)*_4KB);
        terminal_list[i].vidmap = 0;
        map_video_page(terminal_list[i].video_page, terminal_list[i].video_page, 1);
    }
    curr_terminal_num = 0;
    return;
}

/*terminal_switch
 * description: switch current terminal to the next terminal when ALT + F# is pressed
 * input: terminal index for next terminal
 * return: none
 */
void terminal_switch(uint32_t next_terminal_num)
{
    if (curr_terminal_num == next_terminal_num) return;
    memcpy((void*)(terminal_list[curr_terminal_num].video_page), (void*)VIDEO_MEMORY_ADDR, _4KB);       // store current terminal video memory
    memcpy((void*)VIDEO_MEMORY_ADDR, (void*)(terminal_list[next_terminal_num].video_page), _4KB);
    terminal_list[curr_terminal_num].x_cursor = read_screen_x();
    terminal_list[curr_terminal_num].y_cursor = read_screen_y();
    update_video_mem((int8_t*)VIDEO_MEMORY_ADDR);       // use the new video memory
    update_screen_x_y(terminal_list[next_terminal_num].x_cursor, terminal_list[next_terminal_num].y_cursor);
    char* curr_keyb_buf = (char*)get_keyb_buffer();
    int i;
    cli();
    terminal_list[curr_terminal_num].keyb_buffer_index = get_keyb_buffer_index();       // get the terminal informataion from the keyboard
    for (i=0; i< terminal_list[curr_terminal_num].keyb_buffer_index; i++) {
        terminal_list[curr_terminal_num].keyb_buffer[i] = curr_keyb_buf[i];
    }
    sti();
    update_keyb_buf(terminal_list[next_terminal_num].keyb_buffer, terminal_list[next_terminal_num].keyb_buffer_index);
    curr_terminal_num = next_terminal_num;      // change the value of current_terminal
    memory_switch();

}

/*get_display_tid
 * input: none
 * ouput: the tid of current displayed terminal
 * side effect:get current display tid
 */
int get_display_tid ()
{
    return curr_terminal_num;
}

/*set_scheduled_tid
 * input: tid for scheduled terminal
 * output: none
 * side effect:set current scheduled tid
 */
void set_scheduled_tid(int tid)
{
    scheduled_terminal_num = tid;
}

