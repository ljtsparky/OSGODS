#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "types.h"
#define TERMINAL_BUF_MAX 128  //we limit the max buffer size of terminal to be 128 bytes

void terminal_init();
char* get_terminal_buf();
int terminal_open(const uint8_t* name);
int terminal_close(int32_t fd);
int terminal_bad_read_write(int32_t fd, void* buf ,int nbytes);
int terminal_read(int32_t fd, void* buf ,int nbytes);
int terminal_write(int32_t fd, void* buf ,int nbytes);
void set_signal_enable();
#endif
