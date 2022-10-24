// /* types.h - Defines to use the familiar explicitly-sized types in this
//  * OS (uint32_t, int8_t, etc.).  This is necessary because we don't want
//  * to include <stdint.h> when building this OS
//  * vim:ts=4 noexpandtab
//  */

// #ifndef _TYPES_H
// #define _TYPES_H

// #define NULL 0

// #ifndef ASM

// /* Types defined here just like in <stdint.h> */
// typedef int int32_t;
// typedef unsigned int uint32_t;

// typedef short int16_t;
// typedef unsigned short uint16_t;

// typedef char int8_t;
// typedef unsigned char uint8_t;

// #endif /* ASM */

// #endif /* _TYPES_H */

/* types.h - Defines to use the familiar explicitly-sized types in this
 * OS (uint32_t, int8_t, etc.).  This is necessary because we don't want
 * to include <stdint.h> when building this OS
 * vim:ts=4 noexpandtab
 */

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

typedef struct file_jump_table
{
    int32_t (*file_open)(const uint8_t filename);
    int32_t (*file_close)(int32_t fd);
    int32_t (*file_read)(int32_t fd, const void* buf, int32_t nbtypes);
    int32_t (*file_write)(int32_t fd, const void* buf, int32_t nbtypes);

}file_jump_table_t;


typedef struct file_object
{
    file_jump_table_t file_op_tb;
    int32_t inode;
    int32_t file_position;
    int32_t flag;
}file_object_t;

// typedef struct pcb
// {
//     file_object_t fd[MAX_FILE_NUM];
// };

uint32_t file_system_start_address;              /* Starting place for file system */

#endif /* ASM */

#endif /* _TYPES_H */
