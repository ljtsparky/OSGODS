#ifndef _CONTEXT_SWITCH_H
#define _CONTEXT_SWITCH_H

#include "syscall.h"
#include "execute.h"

// #ifndef ASM

extern void context_switch_ASM(uint32_t* eip);

// #endif
#endif
