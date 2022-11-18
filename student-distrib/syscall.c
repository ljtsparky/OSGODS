#include "lib.h"
#include "syscall.h"
#include "types.h"
#include "x86_desc.h"
#include "terminal.h"
#include "rtc.h"
#include "filesystem.h"
#include "execute.h"
#include "page.h"

file_operation_table_t stdin_table = {terminal_open, terminal_read, terminal_bad_write, terminal_close};
file_operation_table_t stdout_table = {terminal_open, terminal_bad_read, terminal_write, terminal_close};
file_operation_table_t rtc_table = {rtc_open, rtc_read, rtc_write, rtc_close};
file_operation_table_t dir_table = {dir_open, dir_read, dir_write, dir_close};
file_operation_table_t file_table = {file_open, file_read, file_write, file_close};
file_operation_table_t fail_table1 = (file_operation_table_t){fail1, fail1, fail1, fail1};
int fail1()
{
    return -1;
}

//read
//description: read data from keyborad, file, RTC, directory
//input: fd - file descriptor, buf - buffer contains data, nbytes - number of bytes
//output: 
//return: the number of bytes read
//side effect: read the data
int32_t read (int32_t fd, void* buf, int32_t nbytes)
{
    if (fd < 0 || fd >= MAX_FILE_NUM ||buf == NULL ||nbytes < 0)//if fd is not in 0 to 7, return -1 judge wheterh buf is valid check the input nbytes
    {   
        return -1;
    }
    pcb_t* curr_pcb = get_current_pcb();
    if (curr_pcb->file_descriptor[fd].flag == 0) {
        return -1;
    }
    int32_t ret = curr_pcb->file_descriptor[fd].file_op_tb.read(fd, buf, nbytes);
    return ret;
}
//write
//description: write data to a terminal or RTC
//input: fd - file descriptor, buf - buffer contains data, nbytes - number of bytes
//output:
//return: number of bytes write or -1 for fail
//side effect: write data
int32_t write (int32_t fd, const void* buf, int32_t nbytes)
{    
    // printf("fd is %d, nbytes %d in write\n",fd,nbytes);
    if (fd < 0 || fd >= MAX_FILE_NUM)
    {
        return -1;
    }
    // judge wheterh buf is valid
    if (buf == NULL)
    {
        return -1;
    }
    // check the input nbytes
    if (nbytes < 0)
    {
        return -1;
    }
    pcb_t* curr_pcb = get_current_pcb();
    if (curr_pcb->file_descriptor[fd].flag == 0)
    {
        return -1;
    }
    return curr_pcb->file_descriptor[fd].file_op_tb.write(fd, buf, nbytes);

}

//open
//description: provide access to file system
//input: filename
//return: 0 for success, -1 for fail
int32_t open (const uint8_t* filename){
    int i;
    int fd = 0;
    int type;
    dentry_t dentry;
    // printf("in open\n");
    pcb_t* cur_pcb = get_current_pcb();
    //looking for avalibale space in File discriptor table
    for(i = 0; i < MAX_FILE_NUM; i++){
        if(i == MAX_FILE_NUM-1 && cur_pcb->file_descriptor[i].flag == 1){return -1;}
        if(cur_pcb->file_descriptor[i].flag == 0){ fd = i; break; }
    }

    if(filename == NULL ){return -1;} //file name invalid
    if( read_dentry_by_name(filename, &(dentry)) != 0 ){return -1;} //file doesn't exist
    type = dentry.file_type;
    if( type < RTC_TYPE || type >FILE_TYPE){return -1;} //file type invalid
    cur_pcb->file_descriptor[fd].inode = (type == FILE_TYPE)? dentry.inode_id  : 0;
    cur_pcb->file_descriptor[fd].file_position    = 0;
    cur_pcb->file_descriptor[fd].flag       = 1;

    // printf("filename is %s\n", filename);
    // printf("type is : %d\n",type);
    switch (type) {
    case RTC_TYPE:
        cur_pcb->file_descriptor[fd].file_op_tb  = rtc_table;
        cur_pcb->file_descriptor[fd].inode=-1;
        break;
    case DIR_TYPE:
        cur_pcb->file_descriptor[fd].file_op_tb  = dir_table;
        break;
    case FILE_TYPE:
        cur_pcb->file_descriptor[fd].file_op_tb  = file_table;
        break;
    }

    cur_pcb->file_descriptor[fd].file_op_tb.open(filename);
    // printf("open fd number is %d\n",fd);
    return fd;
}

//close
//description: close the file descriptor
//input: file descriptor
//output:
//return: 0 for success, -1 for fail
//side effect:
int32_t close (int32_t fd)
{
    pcb_t* curr_pcb = get_current_pcb();
    if (fd==0 || fd==1){
        return -1;
    }
    if (fd < MAX_FILE_NUM && fd >= NUM_IO){
        if (curr_pcb->file_descriptor[fd].flag == 0){
            return -1;
        }
        curr_pcb->file_descriptor[fd].flag = 0;
        curr_pcb->file_descriptor[fd].file_op_tb =  (file_operation_table_t) fail_table1;
        curr_pcb->file_descriptor[fd].inode = 0;
        curr_pcb->file_descriptor[fd].file_position = 0;
        return 0;
    }
    else{
        return -1;
    }
}

#define _128MB 0x8000000
#define _132MB   0x8400000
#define USER_MAP_LOCATION 0x84B8000
#define _4KB_PAGE_TOP_10_OFFSET 22
/*getargs(uint8_t* buf, int32_t nbytes)
* description: read the commmand to a user buffer
* input: buffer: the user buffer to store the data
* return: 0 for success, -1 for fail
*/
int32_t getargs (uint8_t* buf, int32_t nbytes)
{
    /* determine whether args in user space or not */
    if (buf == NULL || nbytes == 0)
        return -1;
    if ((int)buf + nbytes > _132MB - 4 || (int)buf < _128MB  )
        return -1;
    pcb_t* pcb = get_current_pcb();
    /* check the valid of args*/
    if (pcb->args[0] == '\0')
        return -1;
    strncpy((int8_t*)buf, (int8_t*)pcb->args, nbytes);    /* copy args from pcb to buf */
    return 0;
}

/*vidmap(uint8_t** screen_start)
* description: map the video memory to the virtual memory
* input: screen_start: start of virtual memory
* return: 0 for success, -1 for fail
*/
int32_t vidmap (uint8_t** screen_start)
{
    if (screen_start == NULL){
        return -1;
    }
    // judge whether screen start in the user space
    if ((int)screen_start < _128MB || (int)screen_start > _132MB - 4 )
    {
        return -1;
    }
    // if ((int32_t)screen_start>>_4KB_PAGE_TOP_10_OFFSET  < 32 || (int32_t)screen_start>>_4KB_PAGE_TOP_10_OFFSET >= (32+1)){return -1;}
    pcb_t* pcb = get_current_pcb();
    set_up_paging(pcb->pid);
    set_user_video_map();
    *screen_start = (uint8_t*)_132MB;
    return 0;
}


int32_t set_handler (int32_t signum, void* handler_address)
{
    return 0;
}

int32_t sigreturn (void)
{
    return 0;
}

