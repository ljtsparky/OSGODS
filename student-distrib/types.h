#ifndef _TYPES_H
#define _TYPES_H

#define NULL 0

#ifndef ASM

/* Types defined here just like in <stdint.h> */
typedef int int32_t;
typedef unsigned int uint32_t;

typedef short int16_t;
typedef unsigned short uint16_t;

typedef char int8_t;
typedef unsigned char uint8_t;

#define PCB_SIZE 400
#define MAX_FILE_NUM 8
#define MIN_FILE_NUM 0

#include "lib.h"

typedef struct file_operation_table
{
    int32_t (*open)(const uint8_t* name);
    int32_t (*read)(int32_t fd, void* buf, int32_t nbytes);
    int32_t (*write)(int32_t fd, const void* buf, int32_t nbytes);
    int32_t (*close)(int32_t close);
} file_operation_table_t;

typedef struct file_object
{
    file_operation_table_t file_op_tb;
    int32_t inode;
    int32_t file_position;
    int32_t flag;
} file_object_t;

uint32_t file_system_start_address;              /* Starting place for file system */

typedef struct pcb{
    uint32_t pid;
    uint32_t parent_id;
    file_object_t file_descriptor[MAX_FILE_NUM];  
    int32_t saved_esp;
    int32_t saved_ebp;
    uint32_t esp0;
    uint32_t active;
    uint8_t* argument;
    uint32_t vidmap;
    uint32_t tss_esp0;
    int tid; //which tid was it created?
    // int terminal_pid; //which terminal pid
} pcb_t;

typedef struct terminal {
    uint32_t tid;
    int8_t* video_page;
    int x_cursor;
    int y_cursor;
    pcb_t*    pcb_ptr;
    uint32_t  count_pcb;
    char*   keyb_buffer;
    int32_t  keyb_buffer_index;
    void*   next_terminal;
    int vidmap;
    int prev_screen_x[25];    /* store the previous x location when '\n' is pressed */
    int i_prev_x;               /* prev x location index*/
    char display_buf[80*25];   /* display buffer */
    int display_buf_index ;      /* index for display input buffer */
    int line_is_full[25];  
} terminal_t;

terminal_t  terminal_list[3];   /* we have 3 terminal in total */
// terminal_t* curr_terminal_ptr = NULL;

#endif /* ASM */

#endif /* _TYPES_H */
