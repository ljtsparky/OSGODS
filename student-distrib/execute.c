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
#include "context_switch.h"

file_operation_table_t stdin_table1 = (file_operation_table_t){terminal_open, terminal_read, terminal_bad_write, terminal_close};
file_operation_table_t stdout_table1 = (file_operation_table_t){terminal_open, terminal_bad_read, terminal_write, terminal_close};
file_operation_table_t fail_table = (file_operation_table_t){fail, fail, fail, fail};
int fail()
{
    return -1;
}

#define MAX_ARGS_SIZE 128
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
int pid = 0;    /* process id */
pcb_t* curr_pcb = NULL;

int count_how_many_shell_on=0;
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
   
    printf("command is %s\n",command);
    if  ( strncmp((int8_t*)command, "shell", 5) == 0 ) { //this means command is shell
        printf("in shell now\n");
        if (pid==MAX_PCB){
            printf("6 pcbs are already on, can't open new shell\n");
            return 0;
        }
    } 
    /* ------------------------------ parse cmd -------------------------------------------------*/
    uint8_t filename[FILE_NAME_SIZE];
    uint8_t args[MAX_ARGS_SIZE];  
    if (parse_cmd(command, filename, args)) return -1;  /* parse command fails */
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
    create_pcb(new_pcb, pid, args);

    // printf("pid number is %d\n", pid);
    curr_pcb = new_pcb;
    // printf("go to context switch !/n");
    /* ------------------------------ prepare for context switch ---------------------------------*/
    pid ++;

    // printf("before context switch \n");

    if (curr_pcb->pid> 0){
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
    sti();
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
    cli();
    printf("in halt\n");
    if (curr_pcb->parent_id == -1 ){
        printf("we can't exit base!\n");
        pid = 0;
        execute((uint8_t*)"shell");
    }
    pcb_t* parent_pcb = (pcb_t*)(_8MB - _8KB * (curr_pcb->parent_id + 1));//get parent pcb
    //------------------ Restore parent paging ---------------------------------
    int i;   //for iteration
    for (i = 2; i < MAX_FILE_NUM; i++){
        if( curr_pcb->file_descriptor[i].flag == 1 ){  close(i); }
        // close(i);
    }
    curr_pcb->file_descriptor[0].flag = 0;
    curr_pcb->file_descriptor[1].flag = 0;    

    set_up_paging( curr_pcb->parent_id );
    pid--; 
    tss.esp0 = parent_pcb->esp0;
    tss.ss0 = KERNEL_DS;
    uint32_t parent_esp = parent_pcb->saved_esp;
    uint32_t parent_ebp = parent_pcb->saved_ebp;
    curr_pcb = parent_pcb;
    //------------------ Jump to execute return --------------------------------
    uint16_t retval = (uint16_t) status;
    // if(status == 0x0F) retval = 256;
    asm volatile(
                "movl %1, %%esp ;"
                "movl %2, %%ebp ;"
                "xorl %%eax,%%eax;"
                "movw %0, %%ax ;"
                 "leave;"
                 "ret;"
                 :
                 :"r"((uint16_t)retval), "r"(parent_esp), "r"(parent_ebp)
                 :"eax","esp", "ebp"
                 );
    printf("halt last\n");
    return 0;

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
        if (buf_index >= MAX_ARGS_SIZE){
            return -1;
        }
        buf[buf_index++] = command[i];
    }
    buf[buf_index] = '\0';
    return 0;
}

/*function: create_pcb
* input: pcb, pid
* output: none
* side effect: create a new pcb
*/
void create_pcb(pcb_t* pcb, int pid, uint8_t* args)
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
    pcb->args = args;
    // strncpy((int8_t*)(pcb->args), (int8_t*)args, MAX_ARGS_SIZE);
    pcb->active = 0;
    pcb->esp0 = _8MB - _8KB * (pid) - 4;  // 4 bytes for align
    if ( pid == 0) {
        pcb->parent_id = -1;
    }
    else{
        pcb->parent_id = curr_pcb->pid;
    }
    
}

/*function: context_switch
* input: pcb, entry_ptr
* output: none
* side effect: switch to a new process
*/
void context_switch(pcb_t* pcb, uint8_t* entry_ptr){
    tss.esp0 = pcb->esp0;
    tss.ss0 = KERNEL_DS;
    uint32_t XSS = USER_DS;
    uint32_t ESP = _128MB + _4MB - _4_bytes_align;
    uint32_t XCS = USER_CS;
    uint32_t EIP = * (uint32_t*) entry_ptr;
    asm volatile(
        "movw  %%ax, %%ds;"
        "pushl %%eax;"
        "pushl %%ebx;"
        "pushfl  ;"
        "pushl %%ecx;"
        "pushl %%edx;"
        "IRET"
        :
        : "a"(XSS), "b"(ESP), "c"(XCS), "d"(EIP)
        : "cc", "memory"
    );
}


/*function; get_current_pcb
* input:none
* output: a pointer to current pcb
*/
pcb_t* get_current_pcb(){
    return curr_pcb;
}
