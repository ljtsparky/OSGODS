#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "types.h"
#define MAX_TERMINAL_NUM 3
#define KEYB_BUFFER_SIZE 128
#define VIDEO_MEMORY_ADDR 0xB8000
#define _4KB    0x1000

void terminal_init();
void terminal_switch(uint32_t next_terminal);
terminal_t* get_terminal_buf();
int get_display_tid();
int32_t terminal_open(const uint8_t* name);
int32_t terminal_close(int32_t fd);
int32_t terminal_bad_read(int32_t fd, void* buf ,int32_t nbytes);
int32_t terminal_bad_write(int32_t fd, const void* buf ,int32_t nbytes);
int32_t terminal_read(int32_t fd, void* buf ,int32_t nbytes);
int32_t terminal_write(int32_t fd, const void* buf ,int32_t nbytes);
void set_signal_enable();
void set_scheduled_tid(int tid);
extern void memory_switch1();
#endif
