#ifndef _EXECUTE_H
#define _EXECUTE_H

#define _128MB 0x8000000
#define _4MB   0x400000
#define _4_bytes_align 4
extern int32_t execute (const uint8_t* command);
extern int parse_cmd (const uint8_t* command, uint8_t* filename, uint8_t* buf);
extern void create_pcb(pcb_t* pcb, int pid, uint8_t* argument);
extern void context_switch(pcb_t* pcb, uint8_t* entry_ptr);
int32_t halt (uint8_t status);
pcb_t* get_current_pcb();
void scheduler();
extern int determine_which_pcb_empty();
void memory_switch();
extern int fail();
int get_scheduled_tid();
#endif
