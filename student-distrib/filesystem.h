#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

#include "types.h"

#define NUM_INODE   63      // the max number of inode is 63
#define BLOCK_SIZE 4096     // size of one block is 4096
#define FILE_NAME_SIZE 32   // max file name size is 32

// struct of dentry, contain 4 properties
typedef struct dentry_t
{
    uint8_t file_name[32];
    int32_t file_type;
    int32_t inode_id;
    int8_t reserved[24]; 
}dentry_t;

// struct of boot_block, contain 5 properties
typedef struct boot_block_t
{
    int32_t num_dir_entries;
    int32_t num_inodes;
    int32_t num_data_blocks;
    int8_t reserved[52];
    struct dentry_t dir_entries[63];
}boot_block_t;


// struct of inode, contain 2 properties
typedef struct inode_t
{
    int32_t length;
    int32_t data_blocks[1023];
}inode_t;

// varible defination
file_object_t FILE_ARRAY[8];      // max 8 files;
inode_t* inodes; 
boot_block_t* boot_block;
dentry_t* DENTRY;
uint8_t *data_block;
dentry_t global_dentry;

// functions
void file_system_init(int32_t fs_start);
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

int32_t file_open(const uint8_t* filename);
int32_t file_close(int32_t fd);
int32_t file_read(int32_t fd, const void* buf, int32_t nbytes);
int32_t file_write(int32_t fd, const void* buf, int32_t nbytes);

int32_t dir_open(const uint8_t* filename);
int32_t dir_close(int32_t fd);
int32_t dir_read(int32_t fd, const void* buf, int32_t nbytes);
int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes);

#endif
