#ifndef _SYSCALL_HANDLER_H
#define _SYSCALL_HANDLER_H

#include "syscall.h"
#include "execute.h"

#ifndef ASM

extern void context_switch_ASM(uint32_t* eip);

#endif
#endif