#include "fill_idt.h"

void fill_idt_entries(){
    int i;
    for (i=0;i<32;i++){//first 32 are designed by intel and 
        idt[i].reserved3
    }
}