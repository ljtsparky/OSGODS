#include "lib.h"
#include "syscall.h"
#include "types.h"
#include "x86_desc.h"
#include "terminal.h"
#include "rtc.h"
#include "filesystem.h"
#include "execute.h"
#include "page.h"

file_operation_table_t stdin_table = {terminal_open, terminal_read, terminal_bad_read_write, terminal_close};
file_operation_table_t stdout_table = {terminal_open, terminal_bad_read_write, terminal_write, terminal_close};
file_operation_table_t rtc_table = {rtc_open, rtc_read, rtc_write, rtc_close};
file_operation_table_t dir_table = {dir_open, dir_read, dir_write, dir_close};
file_operation_table_t file_table = {file_open, file_read, file_write, file_close};

//read
//description: read data from keyborad, file, RTC, directory
//input: fd - file descriptor, buf - buffer contains data, nbytes - number of bytes
//output: 
//return: the number of bytes read
//side effect: read the data
int32_t read (int32_t fd, void* buf, int32_t nbytes)
{
    
    pcb_t* curr_pcb = get_current_pcb();
    printf("read fd is %d\n", fd);
    sti();
    // int type = (rtc_table.read == curr_pcb->file_descriptor[fd].file_op_tb.read) ? 0 : 1;
    
    // int length = ((inode_t*)(file_system_start_address + BLOCK_SIZE*(1+curr_pcb->file_descriptor[fd].inode) ))->length;

    sti();
    if (fd < 0 || fd >= MAX_FILE_NUM ||buf == NULL ||nbytes < 0)//if fd is not in 0 to 7, return -1 judge wheterh buf is valid check the input nbytes
    {   
        return -1;
    }

    if (curr_pcb->file_descriptor[fd].flag == 0) {
        return -1;
    }
    printf("%d",curr_pcb->file_descriptor[fd].file_position);

    // cli();
    int32_t ret = curr_pcb->file_descriptor[fd].file_op_tb.read(fd, buf, nbytes);
    
    // curr_pcb->file_descriptor[fd].file_position += ret;
    // printf("read end, \n");
    // sti();
    return ret;
    
    // if( type  &&  ( curr_pcb->file_descriptor[fd].file_position >= length ) ){
    //     //printf("********************************************** \n");
    //     return 0;
    // }
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
    if (curr_pcb->file_descriptor[fd].flag != 1)
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
    int fd = -1;
    int type;
    dentry_t dentry;
    printf("in open\n");
    pcb_t* cur_pcb = get_current_pcb();
    if( strlen( (char*)filename ) == 0 ){return -1;} //file name invalid
    if( read_dentry_by_name(filename, &(dentry)) != 0 ){return -1;} //file doesn't exist
    type = dentry.file_type;
    if( type < RTC_TYPE || type >FILE_TYPE){return -1;} //file type invalid

    //looking for avalibale space in File discriptor table
    for(i = 2; i <= MAX_FILE_NUM; i++){
        if(cur_pcb->file_descriptor[i].flag == 0){ fd = i; break; }
        if(i == MAX_FILE_NUM){return -1;}
    }

    cur_pcb->file_descriptor[fd].inode = (type == FILE_TYPE)? dentry.inode_id  : 0;
    cur_pcb->file_descriptor[fd].file_position    = 0;
    cur_pcb->file_descriptor[fd].flag       = 1;


    printf("filename is %s\n", filename);
    printf("type is : %d\n",type);
    switch (type) {
    case RTC_TYPE:
        cur_pcb->file_descriptor[fd].file_op_tb  = rtc_table;
        break;
    case DIR_TYPE:
        cur_pcb->file_descriptor[fd].file_op_tb  = dir_table;
        break;
    case FILE_TYPE:
        cur_pcb->file_descriptor[fd].file_op_tb  = file_table;
        break;
}

    cur_pcb->file_descriptor[fd].file_op_tb.open(filename);
    printf("open fd number is %d\n",fd);
    return fd;
}

// // //open
// // //description: open a file by name, find an empty file descriptor array place and put it in.
// // //input: const filename string
// // //output: it depends, some warning message maybe print out
// // //return: 0 for success, -1 for error case
// // //side effect:
// int32_t open (const uint8_t* filename){
//     int unuse;
//     int type;
//     dentry_t dentry;
//     printf("in open");
//     pcb_t* current_pcb= get_current_pcb();
//     if ( (strlen(filename)==5 && !strncmp(filename,"stdin",5)) || (strlen(filename)==6 && !strncmp(filename,"stdout",6)) ){
//         if (strlen(filename)==5){ // this is stdin
//             if (current_pcb->file_descriptor[0].flag==1){
//                 printf("stdin is already opened at the beginning of entering this pcb\n");
//                 return -1;
//             }
            
//             current_pcb->file_descriptor[0].flag=1;
//             current_pcb->file_descriptor[0].file_op_tb = stdin_table;
//             current_pcb->file_descriptor[0].file_position=0;//only data file will use this?
//             current_pcb->file_descriptor[0].inode=0;//only data file will have inode as non zero?
//             terminal_open("stdin");
//             return 0;
//         }
//         if (strlen(filename)==6){ //this is stdout
//             if (current_pcb->file_descriptor[1].flag==1){
//                 printf("stdout is already opened at the beginning of entering this pcb\n");
//                 return -1;
//             }
            
//             current_pcb->file_descriptor[1].flag=1;
//             current_pcb->file_descriptor[1].file_op_tb = stdout_table;
//             current_pcb->file_descriptor[1].file_position=0;//only data file will use this?
//             current_pcb->file_descriptor[1].inode=0;//only data file will have inode as non zero?
//             terminal_open("stdout");
//             return 0;
//         }
//     }
//     // printf("1111\n");

//     if( strlen( (char*)filename ) == 0 ){return -1;} //file name invalid
//     if( read_dentry_by_name(filename, &dentry) != 0 ){return -1;} //file doesn't exist
//     type = dentry.file_type;
//     if( type < 0 || type >2){return -1;} //file type invalid
//     // printf("type is %d\n",type);
//     //looking for avalibale space in File discriptor table
//     for(unuse = 2; unuse < MAX_FILE_NUM; unuse++){
//         if(current_pcb->file_descriptor[unuse].flag == 0){
//             break;
//         }
//     }
//     // printf("unuse fd is %d\n",unuse);
//     current_pcb->file_descriptor[unuse].inode = (type == 2)? dentry.inode_id  : 0;
//     current_pcb->file_descriptor[unuse].file_position = 0;
//     current_pcb->file_descriptor[unuse].flag=1;
// // printf("4444\n");
//     if (type==0){
//         // current_pcb->file_descriptor[1].flag=1;
//         current_pcb->file_descriptor[unuse].file_op_tb = rtc_table;
//     }
//     if (type==1){
//         // current_pcb->file_descriptor[1].flag=1;
//         current_pcb->file_descriptor[unuse].file_op_tb = dir_table;
//     }
//     if (type==2){
//         // current_pcb->file_descriptor[1].flag=1;
//         current_pcb->file_descriptor[unuse].file_op_tb = file_table;
//     }
//     current_pcb->file_descriptor[unuse].file_op_tb.open(filename);
//     printf("sys open done\n");
    
//     return unuse;
// }
// int32_t open (const uint8_t* filename)
// {
//     printf("in open\n");
//     int i_unuse;
//     pcb_t* curr_pcb = get_current_pcb();
//     for (i_unuse=0;curr_pcb->file_descriptor[i_unuse].flag==1 && i_unuse< MAX_FILE_NUM ; i_unuse++);
//     if (i_unuse==8){return -1;}//this means there are no unused file array place
//     // for the above two lines, we use i_unuse to locate the first unused file descriptor index
//     //now, do differently for different file type?
//     if ( (strlen(filename)==5 && !strncmp(filename,"stdin",5)) || (strlen(filename)==6 && !strncmp(filename,"stdout",6)) ){
//         if (i_unuse>=2){
//             printf("stdin/stdout is already opened at the beginning of entering this pcb\n");
//             return -1;
//         }
//         if (strlen(filename)==5){ // this is stdin
//             terminal_open("stdin");
//             curr_pcb->file_descriptor[i_unuse].flag=1;
//             curr_pcb->file_descriptor[i_unuse].file_op_tb = stdin_table;
//             curr_pcb->file_descriptor[i_unuse].file_position=0;//only data file will use this?
//             curr_pcb->file_descriptor[i_unuse].inode=0;//only data file will have inode as non zero?
//             return 0;
//         }
//         if (strlen(filename)==6){ //this is stdout
//             terminal_open("stdout");
//             curr_pcb->file_descriptor[i_unuse].flag=1;
//             curr_pcb->file_descriptor[i_unuse].file_op_tb = stdout_table;
//             curr_pcb->file_descriptor[i_unuse].file_position=0;//only data file will use this?
//             curr_pcb->file_descriptor[i_unuse].inode=0;//only data file will have inode as non zero?
//             return 0;
//         }
//     }
//     if ( strlen(filename)==3 && !strncmp(filename,"rtc",3) ){
//         if (rtc_open("rtc")==-1){return -1;}
//         curr_pcb->file_descriptor[i_unuse].flag=1;
//         curr_pcb->file_descriptor[i_unuse].file_op_tb = rtc_table;
//         curr_pcb->file_descriptor[i_unuse].file_position=0;//only data file will use this?
//         curr_pcb->file_descriptor[i_unuse].inode=0;//only data file will have inode as non zero?
//         return rtc_open("rtc");
//     }
//     if ( strlen(filename)==1 && !strncmp(filename,".",1)){//the file is directory
//         if (dir_open(".")==-1){return -1;}
//         curr_pcb->file_descriptor[i_unuse].flag=1;
//         curr_pcb->file_descriptor[i_unuse].file_op_tb = dir_table;
//         curr_pcb->file_descriptor[i_unuse].file_position=0;//only data file will use this?
//         curr_pcb->file_descriptor[i_unuse].inode=0;//only data file will have inode as non zero?
//         return dir_open(".");
//     }
//     else{ //in this case, regular data file
//         if (file_open(filename)==-1){return -1;}
//         curr_pcb->file_descriptor[i_unuse].flag=1;
//         curr_pcb->file_descriptor[i_unuse].file_op_tb = file_table;
//         curr_pcb->file_descriptor[i_unuse].file_position = 0;//only data file will use this?
//         curr_pcb->file_descriptor[i_unuse].inode = global_dentry.inode_id;//only data file will have inode as non zero?
//         return 0;
//     }
// }

//close
//description: close the file descriptor
//input: file descriptor
//output:
//return: 0 for success, -1 for fail
//side effect:
int32_t close (int32_t fd)
{
    printf("in close \n");
    pcb_t* curr_pcb = get_current_pcb();
    if (fd<MAX_FILE_NUM && fd>=2){
        curr_pcb->file_descriptor[fd].file_op_tb.close(fd);
        // if (curr_pcb->file_descriptor[fd].flag == 0){
        //     printf("this file is not opened");
        //     return 0;
        // }
        curr_pcb->file_descriptor[fd].flag = 0;
        curr_pcb->file_descriptor[fd].file_op_tb.read = 0;
        curr_pcb->file_descriptor[fd].file_op_tb.open = 0;
        curr_pcb->file_descriptor[fd].file_op_tb.write = 0;
        curr_pcb->file_descriptor[fd].file_op_tb.close = 0;
        curr_pcb->file_descriptor[fd].inode = 0;
        curr_pcb->file_descriptor[fd].file_position = 0;
        return 0;
    }
    else{
        return -1;
    }
}



int32_t getargs (uint8_t* buf, int32_t nbytes)
{
    return 0;
}

int32_t vidmap (uint8_t** screen_start)
{
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

