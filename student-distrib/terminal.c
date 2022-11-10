#include "lib.h"
#include "keyboard.h"
#include "terminal.h"

int num_bytes_to_copy=0;
char terminal_buf[TERMINAL_BUF_MAX];
int terminal_buf_index;

int terminal_bad_read(int32_t fd, void* buf ,int nbytes){
    printf("terminal bad read!!\n");
    return -1;
}
int terminal_bad_write(int32_t fd, const void* buf ,int nbytes){
    printf("terminal bad write!!\n");
    return -1;
}
//terminal_init
//like the open, we initialize the buffer content and index of buffer to 0
//input : none   output:none  return : none  no side-effect
void terminal_init(){
    int i;
    for(i=0;i<TERMINAL_BUF_MAX;i++){
        terminal_buf[i]=0;
    }
    terminal_buf_index=0;
    printf("terminal initialized!\n");
    return;
}

//get_terminal_buf
//input: none
//output:none
//return : return the pointer to the 128 char array terminal_buf
//side effect: none
char* get_terminal_buf(){
    return terminal_buf;
}

//terminal_open
//by opening, we want the terminal buffer set to all zero inside and make the index back to 0
//input: none   output: terminal opened msg   return: 0 for success   no side-effect
int32_t terminal_open(const uint8_t* name){
    int i;
    for(i=0;i<TERMINAL_BUF_MAX;i++){
        terminal_buf[i]=0;
    }
    terminal_buf_index=0;
    printf("terminal opened!\n");
    return 0;
} 

//terminal_close
//clear the buffer, set 0 of index count
//input: none    output: none    return: 0 for success    no side-effect
int32_t terminal_close(int32_t fd){
    int i;
    for (i=0;i<TERMINAL_BUF_MAX;i++){
        terminal_buf[i]=0;
    }
    terminal_buf_index=0; //reset index to zero
    return 0;
}

volatile int terminal_read_enable_signal = 0;

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
     
    printf("in terminal read 1\n");
    while(!terminal_read_enable_signal);
    printf("in terminal read 2\n");
    terminal_read_enable_signal = 0;
    
    if (get_keyb_buffer_index()>=TERMINAL_BUF_MAX){ //make sure the bytes we read is no bigger than max buffer size
        printf("too much to read!! ");
        clean_keyb_buffer();
        return 0;
    }
    //while the enter is not pressed, wait
    printf("in terminal_read now\n"); 
    
    char* keyb_buf = get_keyb_buffer();//get the keyboard buffer pointer
    char* buf1 = (char*) buf;
    
    int i;
    for (i=0; i < get_keyb_buffer_index(); i++) { //i is the iterator and counts how many bytes are written
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
    // printf("shell>> "); //mark
    for (i=0; i<nbytes; i++) { //go through the buffer pointer first nbytes
        putc(buf1[i]);
    }
    return i;
}

//this function is not used now, maybe useful in the future
int get_terminal_buf_index(){
    return terminal_buf_index;
}
