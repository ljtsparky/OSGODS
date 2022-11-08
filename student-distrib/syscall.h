#ifndef _SYSCALL_H
#define _SYSCALL_H
#include "types.h"
#define RTC_TYPE 0
#define FILE_TYPE 2
#define DIR_TYPE 1
int32_t read (int32_t fd, void* buf, int32_t nbytes);
int32_t write (int32_t fd, const void* buf, int32_t nbytes);
int32_t open (const uint8_t* filename);
int32_t close (int32_t fd);
int32_t getargs (uint8_t* buf, int32_t nbytes);
int32_t vidmap (uint8_t** screen_start);
int32_t set_handler (int32_t signum, void* handler_address);
int32_t sigreturn (void);

#endif
