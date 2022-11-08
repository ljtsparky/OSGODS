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

file_operation_table_t stdin_table1 = (file_operation_table_t){terminal_open, terminal_read, terminal_bad_read_write, terminal_close};
file_operation_table_t stdout_table1 = (file_operation_table_t){terminal_open, terminal_bad_read_write, terminal_write, terminal_close};
file_operation_table_t fail_table = (file_operation_table_t){fail, fail, fail, fail};

int fail()
{
    return -1;
}

#define EXE_FILE_LEN    40
#define EXE_MAGIC_TABLE_LEN 4
#define ENTRY_POINT_LEN 4
#define ENTRY_POINT_START   24
#define PROGRAM_IMAG_ADDR   0x08048000
#define STDIN   0
#define STDOUT  1
#define USER_ESP   0x8400000
#define _8MB    0x800000
#define _8KB    0x2000 //0x10 0000 0000 0000
#define MAX_PCB 4
uint8_t exe_magic_table[EXE_MAGIC_TABLE_LEN] = {0x7f, 0x45, 0x4c, 0x46};
pcb_t* pcb_list[MAX_PCB];
int pid = 0;    /* process id */
pcb_t* curr_pcb = NULL;
pcb_t* parent_pcb = NULL;
int total_pcb_num=0;

/*function: execute
* input: command 
* output: 0-255 for halt, 256 for exception, -1 for fail
* side effect: load and execute a new program
*/
int32_t execute (const uint8_t* command)
{
    if (command == NULL) return -1; /* empty command, fail */
    printf("command is %s\n",command);
    // delay(100);
    if (!strncmp(command, "shell", 5)) { 
        printf("in shell now\n");
        curr_pcb = NULL;
        parent_pcb = NULL;
        total_pcb_num = 0;
        pid = 0;//if first five command is not shell, set pid=0
    } 
    
    /* ------------------------------ parse cmd -------------------------------------------------*/
    uint8_t filename[FILE_NAME_SIZE];
    uint8_t* args;  
    //printf("start parse\n");
    if (parse_cmd(command, filename, args)) return -1;  /* parse command fails */
    sti();

    /* ------------------------------ check executable -------------------------------------------*/
    dentry_t dentry;//directory entry
    uint8_t buf[EXE_FILE_LEN]; /* buffer to contain the data */

    if (read_dentry_by_name(filename, &dentry)) return -1; /* finding files fails */
    // printf("after read_dentry_by_name \n");
    
    //printf("filename is: %s\n",filename);
    
    if (EXE_FILE_LEN != read_data(dentry.inode_id, 0, buf, EXE_FILE_LEN)) return -1;   /* reading data fails*/
    // printf("after read data check\n");

    if (strncmp(buf, exe_magic_table, EXE_MAGIC_TABLE_LEN)) return -1;  /* the file is not executable */
    // printf("after magic number check\n");

    uint8_t entry_point_buf[ENTRY_POINT_LEN];
    /* load the entry point */
    if (ENTRY_POINT_LEN != read_data(dentry.inode_id, ENTRY_POINT_START, entry_point_buf, ENTRY_POINT_LEN)) return -1;  /* rading data fails*/
    // printf("after entry point\n");
   
    /* ------------------------------ set up paging ----------------------------------------------*/
    set_up_paging(pid);
    // printf("after paging\n");

    /* ------------------------------ load file into memory --------------------------------------*/
    if (inodes[dentry.inode_id].length !=read_data(dentry.inode_id, 0, PROGRAM_IMAG_ADDR, inodes[dentry.inode_id].length)) return -1;
    // read_data(dentry->inode_id, 0, PROGRAM_IMAG_ADDR, 0x400000);
    // printf("after loading file\n");
    /* ------------------------------ execute create pcb -----------------------------------------*/
    pcb_t new_pcb;
    create_pcb(&new_pcb, pid);
    // printf("pcb done!\n");

    pcb_t* pcb_target_addr = (pcb_t*)(_8MB - _8KB * (pid + 1));
    // printf("pid number is %d\n", pid);
    // printf("before addr \n");
    // *pcb_target_addr = *curr_pcb; 
    memcpy(pcb_target_addr, &new_pcb, 162);
    printf("pid number is %d\n", pid);

    curr_pcb = &new_pcb;

    // printf("go to context switch !/n");

    /* ------------------------------ prepare for context switch ---------------------------------*/
    // context_switch(pcb_list[pid++], entry_point_buf);
    pid ++;
    total_pcb_num ++;
    //printf("before context switch \n");
    context_switch(curr_pcb, entry_point_buf);
    
    //printf("context switch finished\n");
    uint32_t return_value;//return is determined by halt?
    asm volatile(
        "halt_return:"
        "movl %%eax, %0;"
        :"=g"(return_value)       
    );
    printf("return value from halt got, now returning");
    printf("return value %d\n", return_value);
    return return_value;

}

/* function: halt
* input: status
* return value: always 0 for success
* side effect: close the process, restore the parent data and maping 
*/
int32_t halt (uint8_t status){

    int i;   //for iteration
    printf("in halt\n");
    cli();

    pcb_t* pcb_cur = get_current_pcb();
    pcb_t* pcb_prev = pcb_cur->parent_id;


    /* set all present flags in PCB to "Not In Use" to close all the relative fd */
    for (i = 0; i < MAX_FILE_NUM; i++){
        if( pcb_cur->file_descriptor[i].flag == 1 ){  close(i); }
        // close(i);
        pcb_cur->file_descriptor[i].file_op_tb = fail_table;
        pcb_cur->file_descriptor[i].flag = 0;
    }

    printf( "pid: %d parent_id: %d", pcb_cur->pid, pcb_cur->parent_id);

    /* check if the current shell is base shell */
    if (pcb_cur->pid != pcb_cur->parent_id ){
        pid=0;
        unmap_paging();
        execute((uint8_t *)"shell");     //if it is the base shell, re-spawn a new base shell
    }

    //------------------ Restore parent paging ---------------------------------
    set_up_paging( pcb_prev->pid );

    tss.esp0 = pcb_cur->saved_esp;

    sti();

    //------------------ Jump to execute return --------------------------------
    asm volatile(
                 ""
                 "mov %0, %%eax;"
                 "mov %1, %%esp;"
                 "mov %2, %%ebp;"
                 "jmp halt_return;"
                 :
                 :"r"((uint32_t)status), "r"(pcb_cur->saved_esp), "r"(pcb_cur->saved_esp)
                 :"%eax"
                 );
    return 0;

}

//halt1
//description:
//input:
//output:
//return:
//side effect:
int32_t halt1 (uint8_t status)
{
    //printf("now in halt\n");
    cli();
    //if (status = EXCEPTION_NUMBER){    
    //}
    /*Check if base shell*/

    execute("shell");
    
    curr_pcb = get_current_pcb();
    
    curr_pcb -> active = 0;

    /* Restore parent data*/
    int i;
    uint32_t parent_process_id = curr_pcb -> parent_id;
    uint32_t parent_esp = curr_pcb -> saved_esp;
    uint32_t parent_ebp = curr_pcb -> saved_ebp;

   
    /*close relevant fd*/
    for (i = 0; i < MAX_FILE_NUM; i++){
        //curr_pcb -> file_descriptor[i].flag = 0;
        close(i);
    }

    /*Restore parent paging*/
    tss.ss0 = KERNEL_DS;
    tss.esp0 = parent_esp;
    // set_up_paging(parent_process_id);

    // if (curr_pcb -> pid == curr_pcb->parent_id){
    //     execute((uint8_t*)"shell");
    // }
    /*jump to execute return*/
    asm volatile(
        "movl %2, %%ebp;"
        "movl %1, %%esp;"
        "movl %0, %%eax;"
        "jmp halt_return;"
        :
        :"r"((uint32_t)status), "r"(parent_esp), "r"(parent_ebp)
        :"%eax"
    );
}
//parse_cmd
//description: make the first part of command regardless of ' ' to be the filename, and the second part of command saved in buffer
//input: command string, filename, buffer
//output none
//return: 0
//
int parse_cmd (const uint8_t* command, uint8_t* filename, uint8_t* buf)
{
    int command_len = strlen((char*)command);
    printf("cmd len is : %d\n",command_len);
    int i;  /* index walking through the command */
    int filename_index = 0;

    for (i=0; i<command_len && command[i] == ' '; i++);
    for (; i<command_len && command[i] != ' '; i++) {
        filename[filename_index++] = command[i];
    }
    filename[filename_index] = '\0';
    
    // printf("%s\n",filename);
    // printf("%d",i);
    // for (; i<command_len && command[i] == ' '; i++);
    // for (; i<command_len && command[i] != ' '; i++) {
    //     buf[buf_index++] = command[i];
    // }
    // printf("stop here\n");
    // buf[buf_index] = '\0';
    return 0;
}

/*function: create_pcb
* input: pcb, pid
* output: none
* side effect: create a new pcb
*/
void create_pcb(pcb_t* pcb, int pid)
{
    printf("in create pcb\n");

    printf("0\n");
    int i;
    for (i=0; i<8; i++) {
        pcb->file_descriptor[i].file_op_tb.read = 0;
        pcb->file_descriptor[i].file_op_tb.write = 0;
        pcb->file_descriptor[i].file_op_tb.open = 0;
        pcb->file_descriptor[i].file_op_tb.close = 0;
        pcb->file_descriptor[i].flag = 0;
        pcb->file_descriptor[i].inode = 0;
        pcb->file_descriptor[i].file_position = 0;
    }
    printf("1\n");
    // open("stdin");
    pcb->file_descriptor[STDIN].file_op_tb = (file_operation_table_t)stdin_table1;
    pcb->file_descriptor[STDIN].flag = 1;
    pcb->file_descriptor[STDIN].inode = 0;
    pcb->file_descriptor[STDIN].file_position = 0;

    printf("2\n");
    // open("stdin");
    pcb->file_descriptor[STDOUT].file_op_tb = (file_operation_table_t)stdout_table1;
    pcb->file_descriptor[STDOUT].flag = 1;
    pcb->file_descriptor[STDOUT].inode = 0;
    pcb->file_descriptor[STDOUT].file_position = 0;    
    // parent_pcb = curr_pcb;//update the parent pcb as current because we are going to switch to new created pcb
    // curr_pcb = pcb; //current pcb set to new pcb
    // open("stdin");
    // open("stdout");
    // printf("after open\n");
    printf("3\n");
    pcb->pid =(uint32_t) pid;
    pcb->active = 0;
    // parent_pcb = curr_pcb;//update the parent pcb as current because we are going to switch to new created pcb
    // curr_pcb = pcb; //current pcb set to new pcb
    // curr_pcb->parent_id = parent_pcb->pid;
    printf("4\n");
    pcb->parent_id = (total_pcb_num == 0) ? pid : curr_pcb->pid;

    // total_pcb_num ++;
    printf("5\n");
    asm volatile(
        "movl   %%esp, %%eax;"
        :"=a"(pcb->saved_esp)
    );

    printf("5\n");
    asm volatile(
        "movl   %%ebp, %%ebx;"
        :"=b"(pcb->saved_ebp)
    );
    printf("6\n");
     //initialize other things in pcb structures

    // //pcb.par_esp  = 0;
    // //pcb.par_ebp  = 0;
    // pcb->ker_esp  = 0;
    // pcb->ker_ebp  = 0;
    // pcb->tss_esp0 =  tss.esp0;


}

/*function: context_switch
* input: pcb, entry_ptr
* output: none
* side effect: switch to a new process
*/
void context_switch(pcb_t* pcb, uint8_t* entry_ptr)
{
    tss.ss0 = KERNEL_DS;
    // printf("fsdfsdfsdfdsfnnn\n");
    // printf("val saved esp is: %d\n",pcb->saved_esp);
    // tss.esp0 = pcb->saved_esp;
    tss.esp0 = pcb->saved_esp;
    uint32_t eip = *(uint32_t*) entry_ptr;
    // printf("in context switch!!!!!!!!!!!!\n");
    // printf("%d \n",eip);
    context_switch_ASM(eip);
    printf("switch done!\n");
    // asm volatile(
    //     "pushl  %0;"  
    //     "pushl  %1;"
    //     "pushfl;"
    //     "pushl  %2;"
    //     "movl   %3, %%edx;"
    //     "movl   %0, %%eax;"
    //     "movw   %%ax, %%dx;"
    //     "pushl  %%edx;"
    //     "iret;"

    // // # pops return instruction pointer, return code segment selector, 
    // // # and EFLAGS image from the stack to the EIP, CS, and EFLAGS registers, respectively, 
    // // # and then resumes execution of the interrupted program or procedure. 
    // // # If the return is to another privilege level, the IRET instruction also 
    // // # pops the stack pointer and SS from the stack, before resuming program execution.
    //     :
    //     : "r" (USER_DS),"r" (USER_ESP), "r"(USER_CS), "r"(eip)
    //     : "eax" ,"edx"
    // );   

    //  asm volatile(
    //     // "movw  %%ax, %%ds;"
    //     // "pushl $0x002B, %%ax;"
    //     "pushl %%eax;"
    //     "movl  %%ebx, %%eax;"
    //     "pushl %%eax;"
    //     "pushfl  ;"
        
    //     "popl  %%eax;"
    //     "orl $0x200, %%eax;"
    //     "pushl  %%eax;"
    //     "pushl %%ecx;"
    //     "pushl %%edx;"
    //     // "int $0;"
    //     "IRET;"
    //     :
    //     : "a"(USER_DS), "b"(USER_ESP), "c"(USER_CS), "d"(eip)
    //     : "cc", "memory"
    // );
    // int rval;
    // asm volatile(
    //     "mov $0x2B, %%ax;"
    //     "mov %%ax, %%ds;"
        
    //     "movl $0x83FFFFC, %%eax;"
    //     "pushl $0x2B;"
    //     "pushl %%eax;"
    //     "pushfl;"
    //     "popl %%edx;"
    //     "orl $0x200, %%edx;"
    //     "pushl %%edx;"
    //     "pushl $0x23;"
    //     "pushl %1;"
    //     // "iret;"
    //     // "execute_return:;"
    //     // "movl %%eax, %0;"
    //     : "=g" (rval)
    //     : "r" (eip)
    //     : "edx", "eax"
    // );
   
}

/*function; get_current_pcb
* input:none
* output: a pointer to current pcb
*/
pcb_t* get_current_pcb()
{
    // pcb_t* pcb;
    // asm volatile(
    //      "andl %%esp, %0"
    //      : "=r" (pcb)
    //      : "r" (0xFFFFE000)
    //      );
    // return pcb;
    return curr_pcb;
}

/* function: get_parent_pcb
* input: none
* output: a pointer to the parent pcb
*/
pcb_t* get_parent_pcb()
{
    return parent_pcb;
}

