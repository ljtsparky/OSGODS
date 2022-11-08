#ifndef _EXECUTE_H
#define _EXECUTE_H

int32_t execute (const uint8_t* command);
extern int parse_cmd (const uint8_t* command, uint8_t* filename, uint8_t* buf);
extern void create_pcb(pcb_t* pcb, int pid);
extern void context_switch(pcb_t* pcb, uint8_t* entry_ptr);
int32_t halt (uint8_t status);
pcb_t* get_current_pcb();
pcb_t* get_parent_pcb();


extern int fail();
#endif
