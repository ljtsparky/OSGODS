#include "filesystem.h"
#include "lib.h"
#include "execute.h"
/*
 *name:init_filesys
 *discription:  initialize file system
 *input:        start address of the filesystem -- fs_start
 *output:       void
 */
void file_system_init(int32_t fs_start){
    boot_block = (boot_block_t*) fs_start;
    inodes = (inode_t*) (fs_start + BLOCK_SIZE);        // block size is 4096
    data_block = (uint8_t*) (fs_start + BLOCK_SIZE * (boot_block->num_inodes+1));       
    return;
}

/*
 *name: read_dentry_by_name
 *
 *discription: read the name in the file list, then copy the address to the dentry
 *input:        fname -- the name of the file
 *              dentry -- directory entry
 *output:       0 success, -1 fail
 */
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry){
    int i;
    // int file_name_length;
    for ( i = 0; i < boot_block->num_dir_entries; i++){
        
        if (strncmp((int8_t*)boot_block->dir_entries[i].file_name, (int8_t*)fname, (uint32_t)FILE_NAME_SIZE) == 0 && strlen(fname) <= 32){
            memcpy(dentry, boot_block->dir_entries+i, sizeof(dentry_t));
            return 0;
        }
    }
    return -1;
}

/*
 *name: read_dentry_by_index
 *
 *discription: read the index in the file list, then copy the address to the dentry
 *input:        index -- the index of the file
 *              dentry -- directory entry
 *output:       0 success, -1 fail
 */
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry){
    if (index > boot_block->num_dir_entries - 1)
    {
        return -1;
    }
    memcpy(dentry, boot_block->dir_entries+index, sizeof(dentry_t));
    return 0;
}

/*
 *name: read_data
 *
 *discription: read the data in the file, then copy the length content from offset into the buf
 *input:        inode -- the inode_id of the file
 *              offset -- the start address in the file
 *              buf -- the destination of the file content
 *              length -- copy length
 *output:       0, read_length success, -1 fail
 */
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){
    if (inode > boot_block->num_inodes - 1 || buf == NULL)    {
        return -1;
    }
    int file_length = inodes[inode].length;
    // printf("%d\n",file_length);
    if (offset >= file_length)
        return 0;
    if (offset+length>file_length)
        length = file_length-offset;


    int block_offset = offset / BLOCK_SIZE;     // start block
    int byte_offset = offset %BLOCK_SIZE;       // start byte
    int current_block = inodes[inode].data_blocks[block_offset];      // current block read from inodes
    //printf("show current block %d, byte_offset %d\n", current_block, byte_offset);
    int read_length = length;
    //int i;
    if (read_length<=BLOCK_SIZE-byte_offset)
    {
        //putc('@');
        // read length bytes then return
        memcpy(buf, data_block+current_block*BLOCK_SIZE+byte_offset, read_length);     
        return length;
    }
    // read BLOCKSIZE-byte_offset bytes
    memcpy(buf, data_block+current_block*BLOCK_SIZE+byte_offset, BLOCK_SIZE-byte_offset);
    read_length-=BLOCK_SIZE-byte_offset;
    buf+= BLOCK_SIZE-byte_offset;
    block_offset++;
    current_block = inodes[inode].data_blocks[block_offset];
    while (read_length > BLOCK_SIZE)
    {
        //read a block,
        memcpy(buf, data_block+current_block*BLOCK_SIZE, BLOCK_SIZE);
        read_length-=BLOCK_SIZE;
        block_offset++;
        current_block = inodes[inode].data_blocks[block_offset];
        buf+=BLOCK_SIZE;
    }
    // read length bytes
    memcpy(buf, data_block+current_block*BLOCK_SIZE, read_length);
    return length;

/*
    for ( i = 0; i < length; i++)
    {
        if (block_offset >= boot_block->num_data_blocks)
        {
            return -1;
        }
        memcpy(buf+i, data_block[current_block]+byte_offset, 1);
        byte_offset++;
        if (byte_offset == BLOCK_SIZE)
        {
            byte_offset = 0;
            block_offset++;
            current_block = inodes->data_blocks[block_offset];
        }
        
    }
    return i;
*/
}

//FILE OPERATIONS

/*
 *name: file_open
 *
 *discription: open file with filename
 *intput: filename -- file name
 *output: 0 success, -1 fail
 */
int32_t file_open(const uint8_t* filename)
{
    if (read_dentry_by_name(filename, &global_dentry) == -1)
    {
        return -1;
    }
    return 0;
}

/*
 *name: file_close
 *
 *discription: close file with fd
 *intput: fd -- file discription
 *output: 0 
 */
int32_t file_close(int32_t fd)
{
    return 0;
}

/*
 *name: file_wite
 *
 *discription: write file, always fail
 *intput: fd -- file discription
 *        buf -- the destination address
 *        nbytes -- write length
 *output: -1 
 */
int32_t file_write(int32_t fd, const void* buf, int32_t nbytes)
{
    return -1;
}


/*
 *name: file_read
 *
 *discription: reand file for nbytes length, then copy into the buf
 *intput: fd -- file discription
 *        buf -- the destination address
 *        nbytes -- read length
 *output: readlenth, or 0 for reach end, or -1 fail
 */
int32_t file_read(int32_t fd, void* buf, int32_t nbytes)
{
    //int32_t offset = get_fd_from(fd)->file_position;
    //int32_t inode = get_fd_from(fd)->inode;
    //return read_data(inode, offset, buf, nbtypes);
    //file_object_t file_desc = FILE_ARRAY[fd];
    //int offset = 0;
    printf("in file read\n");
    pcb_t* cur_pcb;
    cur_pcb=get_current_pcb();
    uint32_t inode = cur_pcb->file_descriptor[fd].inode;
    uint32_t offset = cur_pcb->file_descriptor[fd].file_position;
    //uint8_t* buf1= (uint8_t*) buf;
    //
    // uint32_t read_nbytes = read_data(global_dentry.inode_id, offset, buf, nbytes);
    uint32_t read_nbytes = read_data(inode, offset, buf, nbytes);
    if (read_nbytes < 0)
    {
        return -1;
    }
    
    return read_nbytes;
}

// DIR OPERATIONS

/*
 *name: dir_open
 *
 *discription: open dir with name filename
 *input: filename -- dir name
 *output: 0 success, -1 fail
 */
int32_t dir_open(const uint8_t* filename)
{
    if (read_dentry_by_name(filename, &global_dentry) || global_dentry.file_type != 1)
    {
        return -1;
    }
    
    return 0;
}

/*
 *name: dir_close
 *
 *discription: close dir
 *intput: fd -- file discription
 *output: 0 
 */
int32_t dir_close(int32_t fd)
{
    return 0;
}

//dir_read
/*
 *name: dir_read
 *
 *discription: read the dir then copy one filename into buf, then file_position increase 1
 *intput: fd -- file discription
 *        buf -- destination address
 *        nbytes -- length
 *output: 0 success, -1 fail
 */
int32_t dir_read(int32_t fd, void* buf, int32_t nbytes)
{
    dentry_t dentry;
    pcb_t* cur_pcb = get_current_pcb();
    // printf("in read_dir\n");
    uint32_t index =  cur_pcb->file_descriptor[fd].file_position;
    int32_t result = read_dentry_by_index(index, &dentry);
    // printf("result is %d\n",result);
    if(result==0){
        memcpy((uint8_t*)buf, &(dentry.file_name), FILE_NAME_SIZE);
    }
    else
    {
        return 0;
    }
    cur_pcb->file_descriptor[fd].file_position++;
    if (cur_pcb->file_descriptor[fd].file_position < 63)      // max files number
    {
        return nbytes;
    }
    else
    {
        return 0;
    }
    
}

/*
 *name: dir_write
 *
 *discription: write dir, always fail
 *intput: fd -- file discription
 *        buf -- destination address
 *        nbytes -- length
 *output: -1 fail
 */
int32_t dir_write(int32_t fd, const void* buf, int32_t nbtypes)
{
    return -1;
}
