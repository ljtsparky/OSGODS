#ifndef _SYSCALL_HANDLER_H
#define _SYSCALL_HANDLER_H

#include "syscall.h"
#include "execute.h"

#ifndef ASM

int syscall_linkage();
// void context_switch_1(uint32_t* entry_point);

#endif
#endif
