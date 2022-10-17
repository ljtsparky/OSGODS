#include "page.h"
#include "x86_desc.h"

/*
 * init_paging
 *  description: initialize the paging
 *  input: none
 *  output: none
 */
void init_paging ()
{
    printf("try initialize paging\n");
    page_directory_entry_4k_t pde_4k;   /* pde for 4k page */
    pde_4k.Present = 1;     //present true
    pde_4k.pt_Base_address = (uint32_t)page_table >> PAGE_TABLE_RIGHT_OFF;
    pde_4k.Available_use = 0;
    pde_4k.Global_page = 0;
    pde_4k.Page_size = 0;
    pde_4k.Reserved = 0;
    pde_4k.Accessed = 0;
    pde_4k.Cache_disabled = 0;
    pde_4k.Write_through = 0;
    pde_4k.Use_Supervisor = 0;
    pde_4k.Read_Write = 0;

    int j; /* index for page table */
    for (j=0; j<NUM_PTE; j++) {
        // page_table[j].p_Base_address = 0;
        // page_table[j].Global_page = 0;
        // page_table[j].Page_table_attribute_index = 0;
        // page_table[j].Dirty = 0;
        // page_table[j].Accessed = 0;
        // page_table[j].Cache_disabled = 0;
        // page_table[j].Write_through = 0;
        // page_table[j].Use_Supervisor = 0;
        // page_table[j].Read_Write = 0;
        // page_table[j].Present = 0;
        page_table[j].val[0] = 0;
    }
    uint32_t u32_pde_4k = pde_4k.val[0];
    page_dir[0] = u32_pde_4k;
    int i =  VIDEO_MEMORY_ADDR / PAGE_SIZE_4K;
    page_table[i].p_Base_address = VIDEO_MEMORY_ADDR >> PAGE_TABLE_RIGHT_OFF;
    page_table[i].Present = 1;
    page_table[i].Read_Write = 1;


    //initialize the pde_4m
    uint32_t pde_4m = 0; //initialize to all 0
    pde_4m=set_present(pde_4m); //set bit 0 present
    pde_4m=set_page_size_1(pde_4m); //set the page size 1, which is bit 7
    pde_4m=set_page_base_address_1(pde_4m); //set address to be 0x00
    pde_4m=set_read_write_1(pde_4m); 
    page_dir[1] = pde_4m;

    /* set the content of control regisster */
    asm volatile(
        "movl %0, %%eax      /* put the page directory address into eax*/ ;"
        "movl %%eax, %%cr3          /* move the address into cr3 */         ;"

        "movl %%cr4, %%eax          /* put cr4 into eax */         ;"
        "orl $0x00000010 ,%%eax     /* make the bit 4 of eax 1 */  ;"
        "movl %%eax,%%cr4     /* move eax back into cr4, changing the bit 4 of it to 1 allowing mixed page sizes */  ;"      
         
        "movl %%cr0, %%eax          /* put cr0 into eax */                  ;"
        "orl  $0x80000000, %%eax    /* set bit 31 of eax to 1*/             ;"
        "movl %%eax, %%cr0          /* set bit 31 of cr0 to 1 */            "
        :
        : "r" (page_dir)
        : "eax");
    /* assembly adapted from Osdev */

    // asm volatile(
	// 		 	"movl %0, %%eax;"
	// 			"movl %%eax, %%cr3;"
	// 			"movl %%cr4, %%eax;"
	// 			"orl $0x010, %%eax;"
	// 			"movl %%eax, %%cr4;"
	// 			"movl %%cr0, %%eax;"
	// 			"orl $0x80000000, %%eax;"
	// 			"movl %%eax, %%cr0;"
	// 			:
	// 			: "r" (page_dir)
	// 			: "eax"
	// 	);
    printf("paging initialize finish\n");
}

