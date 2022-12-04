#include "lib.h"
#include "execute.h"
#include "syscall.h"
#include "filesystem.h"
#include "types.h"
#include "page.h"
#include "terminal.h"
#include "rtc.h"
#include "x86_desc.h"
#include "syscall_handler.h"
#include "x86_desc.h"
// #include "context_switch.h"

file_operation_table_t stdin_table1 = (file_operation_table_t){terminal_open, terminal_read, terminal_bad_write, terminal_close};
file_operation_table_t stdout_table1 = (file_operation_table_t){terminal_open, terminal_bad_read, terminal_write, terminal_close};
file_operation_table_t fail_table = (file_operation_table_t){fail, fail, fail, fail};
int fail()
{
    return -1;
}

#define MAX_ARGUMENT_SIZE 128
#define EXE_FILE_LEN    40
#define EXE_MAGIC_TABLE_LEN 4
#define TARGET_EIP_LENGTH 4
#define ENTRY_POINT_START   24
#define PROGRAM_IMAG_ADDR   0x08048000
#define STDIN   0
#define STDOUT  1
#define USER_ESP   0x8400000
#define _8MB    0x800000
#define _8KB    0x2000 //0x10 0000 0000 0000
#define MAX_PCB 6
uint8_t exe_magic_table[EXE_MAGIC_TABLE_LEN] = {0x7f, 0x45, 0x4c, 0x46};
pcb_t* pcb_list[MAX_PCB];
int pid = -1;    /* process id */
pcb_t* curr_pcb = NULL;
terminal_t* scheduled_terminal = &terminal_list[0];
int count_how_many_shell_on = 0;
pcb_t* terminal_curr_pcb[3];
// int count_none_shell_pcb=0;
/*function: execute
* input: command 
* output: 0-255 for halt, 256 for exception, -1 for fail
* side effect: load and execute a new program
*/
int32_t execute (const uint8_t* command)
{
    cli();
    if (command == NULL) return -1; /* empty command, fail */
    pid = determine_which_pcb_empty();
    if (pid == -2){//this means no blocks available
        return 0;
    }
    
    printf("command is %s \n",command);
    if  ( strncmp((int8_t*)command, "shell", 5) == 0 ) { //this means command is shell
        printf("in terminal %d now\n", get_scheduled_tid() + 1);
    } 
    /* ------------------------------ parse cmd -------------------------------------------------*/
    uint8_t filename[FILE_NAME_SIZE];
    uint8_t argument[MAX_ARGUMENT_SIZE];  
    if (parse_cmd(command, filename, argument)) return -1;  /* parse command fails */
    /* ------------------------------ check executable -------------------------------------------*/
    dentry_t dentry;//directory entry
    uint8_t buf[EXE_FILE_LEN]; /* buffer to contain the data */

    if (read_dentry_by_name(filename, &dentry)) return -1; /* finding files fails */
    // printf("after read_dentry_by_name \n");
    // printf("filename is: %s\n",filename);
    
    if (EXE_FILE_LEN != read_data(dentry.inode_id, 0, buf, EXE_FILE_LEN)) return -1;   /* reading data fails*/
    // printf("after read data check\n");

    if (strncmp((int8_t*)buf, (int8_t*)exe_magic_table, EXE_MAGIC_TABLE_LEN)) return -1;  /* the file is not executable */
    // printf("after magic number check\n");

    uint8_t target_eip_buffer[TARGET_EIP_LENGTH];
    /* load the entry point */
    if (TARGET_EIP_LENGTH != read_data(dentry.inode_id, ENTRY_POINT_START, target_eip_buffer, TARGET_EIP_LENGTH)) return -1;  /* rading data fails*/
    // printf("after entry point\n");
   
    /* ------------------------------ set up paging ----------------------------------------------*/
    set_up_paging(pid);
    /* ------------------------------ load file into memory --------------------------------------*/
    if (inodes[dentry.inode_id].length !=read_data(dentry.inode_id, 0, (uint8_t*)PROGRAM_IMAG_ADDR, inodes[dentry.inode_id].length)) return -1;
    /* ------------------------------ execute create pcb -----------------------------------------*/
    pcb_t* new_pcb =(pcb_t*) (_8MB - _8KB * (pid+1));
    create_pcb(new_pcb, pid, argument);

    // printf("pid number is %d\n", pid);
    curr_pcb = new_pcb;

    if ( curr_pcb->parent_id != -1 ){//this means this pcb is not base shell of any terminal
        uint32_t parent_pid = curr_pcb->parent_id;
        pcb_t* parent_pcb =(pcb_t*)( _8MB - _8KB * (parent_pid + 1));
        uint32_t parent_esp;
        uint32_t parent_ebp;
        asm volatile(
            "movl %%esp, %0;"
             "movl %%ebp, %1;"
            :"=r" (parent_esp), "=r" (parent_ebp)
        );
        parent_pcb->saved_esp = parent_esp;
        parent_pcb->saved_ebp = parent_ebp;
    }
    // scheduled_terminal->pid = pid;
    // sti();
    context_switch(curr_pcb, target_eip_buffer);  
    printf("context switch finished\n");
    return 0;

}

/* function: halt
* input: status
* return value: always 0 for success
* side effect: close the process, restore the parent data and maping 
*/
int32_t halt (uint8_t status){
    printf("in halt\n");
    scheduled_terminal->count_pcb--;
    if (curr_pcb->parent_id == -1 ){
        printf("we can't exit base!\n");
        if (scheduled_terminal->count_pcb<0){
            asm volatile("int $1");
        }
        curr_pcb->active=0;//close current pcb
        execute((uint8_t*)"shell");
        // scheduled_terminal->count_pcb=1;
        // return 0;
    }
    cli();
    pcb_t* parent_pcb = (pcb_t*)(_8MB - _8KB * (curr_pcb->parent_id + 1));//get parent pcb
    //------------------ Restore parent paging ---------------------------------
    int i;   //for iteration
    for (i = 2; i < MAX_FILE_NUM; i++){
        if( curr_pcb->file_descriptor[i].flag == 1 ){  close(i); }
    }
    curr_pcb->file_descriptor[0].flag = 0;
    curr_pcb->file_descriptor[1].flag = 0;    
    curr_pcb->active = 0;
    set_up_paging( curr_pcb->parent_id );
    
    int display_tid = get_display_tid();

    if (scheduled_terminal->vidmap == 1) { //
        scheduled_terminal->vidmap = 0;
        terminal_list[display_tid].vidmap = 0;
    }

    tss.esp0 = parent_pcb->esp0;
    tss.ss0 = KERNEL_DS;
    uint32_t parent_esp = parent_pcb->saved_esp;
    uint32_t parent_ebp = parent_pcb->saved_ebp;
    curr_pcb = parent_pcb;
    //------------------ Jump to execute return --------------------------------
    uint16_t status_to_return=(uint16_t)status;
    if(status == 15) status_to_return = 256;       //that means program dies by an exception
    asm volatile(
                "movl %1, %%esp ;"
                "movl %2, %%ebp ;"
                "xorl %%eax,%%eax;"
                "movw %0, %%ax ;"
                 "leave;"
                 "ret;"
                 :
                 :"r"((uint16_t)status_to_return), "r"(parent_esp), "r"(parent_ebp)
                 :"eax","esp", "ebp"
                 );
    printf("halt last\n");
    return 0;

}

/*function: create_pcb
* input: pcb, pid
* output: none
* side effect: create a new pcb
*/
void create_pcb(pcb_t* pcb, int pid, uint8_t* argument)
{
    // printf("in create pcb\n");
    int i;
    for (i=0; i<MAX_FILE_NUM; i++) {
        pcb->file_descriptor[i].file_op_tb.read = 0;
        pcb->file_descriptor[i].file_op_tb.write = 0;
        pcb->file_descriptor[i].file_op_tb.open = 0;
        pcb->file_descriptor[i].file_op_tb.close = 0;
        pcb->file_descriptor[i].flag = 0;
        pcb->file_descriptor[i].inode = 0;
        pcb->file_descriptor[i].file_position = 0;
    }
    pcb->file_descriptor[STDIN].file_op_tb = (file_operation_table_t)stdin_table1;
    pcb->file_descriptor[STDIN].flag = 1;
    pcb->file_descriptor[STDIN].inode = 0;
    pcb->file_descriptor[STDIN].file_position = 0;
    pcb->file_descriptor[STDOUT].file_op_tb = (file_operation_table_t)stdout_table1;
    pcb->file_descriptor[STDOUT].flag = 1;
    pcb->file_descriptor[STDOUT].inode = 0;
    pcb->file_descriptor[STDOUT].file_position = 0;    
    pcb->pid =(uint32_t) pid;
    pcb->argument = argument;
    pcb->tss_esp0 = tss.esp0;
    pcb->tid = get_scheduled_tid();
    pcb->active = 1;
    pcb->esp0 = _8MB - _8KB * (pid) - _4_bytes_align;  // 4 bytes for align
    if (scheduled_terminal->count_pcb==0) {
        pcb->parent_id = -1;
    }
    else{
        pcb->parent_id = curr_pcb->pid;
        // pcb->parent_id=scheduled_terminal->pid;
    }
    scheduled_terminal->count_pcb++;
    
}

/*function: context_switch
* input: pcb, entry_ptr
* output: none
* side effect: switch to a new process
*/
void context_switch(pcb_t* pcb, uint8_t* entry_ptr){
    tss.esp0=pcb->esp0;
    tss.ss0=KERNEL_DS;
    uint32_t xss=USER_DS;
    uint32_t esp=_128MB + _4MB - _4_bytes_align;
    uint32_t xcs=USER_CS;
    uint32_t eip=* (uint32_t*) entry_ptr;
    asm volatile(
        "movw  %%ax, %%ds;"
        "pushl %%eax;"
        "pushl %%ebx;"
        "pushfl  ;"
        "orl $0x200,(%%esp);"
        "pushl %%ecx;"
        "pushl %%edx;"
        "iret"
        :
        : "a"(xss), "b"(esp), "c"(xcs), "d"(eip)
        : "cc", "memory"
    );
}


/*function: get_current_pcb
* input:none
* output: a pointer to current pcb
*/
pcb_t* get_current_pcb(){
    return curr_pcb;
}

/*function: determine_which_pcb_empty
* input:none
* output: the index for an empty pcb, or -2 for no empty pcb
*/
int determine_which_pcb_empty()
{
    int i;
    pcb_t* temp_pcb_pt;
    for (i=0; i<MAX_PCB; i++){//go throuth all pcbs, and see which one is empty
        temp_pcb_pt = (pcb_t*)(_8MB-_8KB*(i+1));//get the pcb pointer
        if (temp_pcb_pt->active == 0){//if 0, it means it is available  for use
            return i;
        }
    }
    printf("no pcb block available, 6 are opened!\n");
    return -2;
}

/*function: scheduler
* input:none
* output: none
* side effect: do the scheduling, context switch to next terminal current process block
*/
void scheduler()
{
    scheduled_terminal->pcb_ptr = curr_pcb;//store curr pcb pointer

    if (pid != -1 ) {
        asm volatile(
            "movl   %%esp,  %0;"
            "movl   %%ebp,  %1;"
            : "=r" (curr_pcb->saved_esp), "=r" (curr_pcb->saved_ebp)
        );
        curr_pcb->tss_esp0 = tss.esp0;
    } 
    // scheduled_terminal->scheduled = 0;
    terminal_t* next_terminal_ptr;    
    next_terminal_ptr = (terminal_t*)(scheduled_terminal->next_terminal);
    
    if (next_terminal_ptr == NULL ) return;
    
    scheduled_terminal = next_terminal_ptr;
    set_scheduled_tid(scheduled_terminal->tid);
    // scheduled_terminal->scheduled = 1;

    if (scheduled_terminal->pcb_ptr != NULL)
        curr_pcb = scheduled_terminal->pcb_ptr;
    
    if (scheduled_terminal->count_pcb == 0) { //if it is the first time we go into this terminal, launch the base shell
        execute((uint8_t*)"shell");
    } 
    else {
        set_up_paging(scheduled_terminal->pcb_ptr->pid);
        curr_pcb = scheduled_terminal->pcb_ptr;
        asm volatile(
            "movl   %0,  %%ebp;"
            "movl   %1,  %%esp;"
            :
            : "r" (scheduled_terminal->pcb_ptr->saved_ebp), "r" (scheduled_terminal->pcb_ptr->saved_esp)
            : "esp", "ebp"
        );
        tss.esp0 = scheduled_terminal->pcb_ptr->tss_esp0;
        tss.ss0 = KERNEL_DS;
        asm volatile(
            "leave;"
            "ret;"
        );
    }
    
    // set_up_paging(pid);
}

/*function: memory_switch
* input:none
* output: none
* side effect: switch between different terminal video memory
*/
void memory_switch()
{   
    terminal_t* next_terminal = (terminal_t*)scheduled_terminal->next_terminal;
    int display_tid = get_display_tid();
    if (next_terminal == NULL) return;
    int next_tid = next_terminal->tid;
    update_screen_x_y(terminal_list[next_tid].x_cursor, terminal_list[next_tid].y_cursor); 
    if (next_tid != display_tid) {
        update_video_mem(next_terminal->video_page);
        set_vid_map((int8_t*)next_terminal->video_page, next_terminal->vidmap);
        // set_vid_map((int8_t*)next_terminal->video_page, next_terminal->vidmap);
    }
    else {
        update_video_mem((int8_t*)VIDEO_MEMORY_ADDR);
        set_vid_map((int8_t*)VIDEO_MEMORY_ADDR, next_terminal->vidmap);
        // set_vid_map((int8_t*)VIDEO_MEMORY_ADDR, next_terminal);
        
    }
}

/*function: get_scheduled_tid
* input:none
* output: the tid of the scheduled terminal
*/
int get_scheduled_tid()
{
    return scheduled_terminal->tid;
}

//parse_cmd
//description: make the first part of command regardless of ' ' to be the filename, and the second part of command saved in buffer
//input: command string, filename, buffer
//output none
//return: 0
//
int parse_cmd (const uint8_t* command, uint8_t* filename, uint8_t* buf)
{
    if (buf == NULL) 
        return -1;
    int command_len = strlen((char*)command);
    // printf("cmd len is : %d\n",command_len);
    int i;  /* index walking through the command */
    int filename_index = 0;
    int buf_index = 0;
    for (i=0; i<command_len && command[i] == ' '; i++);
    for (; i<command_len && command[i] != ' '; i++) {
        filename[filename_index++] = command[i];
    }
    filename[filename_index] = '\0';
    
    // printf("parse part 2\n");
    for (; i<command_len && command[i] == ' '; i++);
    for (; i<command_len && command[i]  != ' ' ; i++) {
        if (buf_index >= MAX_ARGUMENT_SIZE){
            return -1;
        }
        buf[buf_index++] = command[i];
    }
    buf[buf_index] = '\0';
    return 0;
}
