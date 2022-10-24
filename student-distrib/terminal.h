#ifndef _TERMINAL_H
#define _TERMINAL_H

#define TERMINAL_BUF_MAX 128  //we limit the max buffer size of terminal to be 128 bytes

void terminal_init();
char* get_terminal_buf();
int terminal_open();
int terminal_close();
int terminal_read(int32_t fd, void* buf ,int nbytes);
int terminal_write(int32_t fd, void* buf ,int nbytes);

#endif
