#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "types.h"
#define TERMINAL_BUF_MAX 128  //we limit the max buffer size of terminal to be 128 bytes

void terminal_init();
char* get_terminal_buf();
int32_t terminal_open(const uint8_t* name);
int32_t terminal_close(int32_t fd);
int32_t terminal_bad_read(int32_t fd, void* buf ,int32_t nbytes);
int32_t terminal_bad_write(int32_t fd, const void* buf ,int32_t nbytes);
int32_t terminal_read(int32_t fd, void* buf ,int32_t nbytes);
int32_t terminal_write(int32_t fd, const void* buf ,int32_t nbytes);
void set_signal_enable();
#endif
