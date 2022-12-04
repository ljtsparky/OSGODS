#include "page.h"
#include "x86_desc.h"
#include "lib.h"

/*
 * init_paging
 *  description: initialize the paging by creating the page dir entry correspoding to 4K and 4M, then the 4K one 
 *  input: none
 *  output: none
 * return: none
 * side_effect: 
 */
void init_paging ()
{  
    page_directory_entry_4k_t pde_4k;   /* pde for 4k page */
    pde_4k.val[0] = 0;
    pde_4k.Present = 1;     //present true
    pde_4k.pt_Base_address = (uint32_t)page_table >> PAGE_TABLE_RIGHT_OFF;//right shift 12 because we have aligned it as 4096, so 12 bits are 4096
   
    int j; /* index for page table */
    for (j=0; j<NUM_PTE; j++) { //initialize the pdt and pt by 0
        page_table[j].val[0] = 0;
        page_dir[j]=0;
    }
    uint32_t u32_pde_4k = pde_4k.val[0];
    page_dir[0] = u32_pde_4k;
    int i =  VIDEO_MEMORY_ADDR / PAGE_SIZE_4K;  /* find the page for video memory */
    page_table[i].p_Base_address = VIDEO_MEMORY_ADDR >> PAGE_TABLE_RIGHT_OFF;   /* load the first 20 bits of video address */
    page_table[i].Present = 1;  /* it present now */
    page_table[i].Read_Write = 1;   
    page_table[i].Use_Supervisor=1;
    //initialize the pde_4m
    uint32_t pde_4m = 0; //initialize to all 0
    pde_4m = set_present(pde_4m); //set bit 0 present
    pde_4m = set_page_size_1(pde_4m); //set the page size 1, which is bit 7
    pde_4m = set_page_base_address_1(pde_4m); //set address to be 1, which is the second 4MB
    pde_4m = set_read_write_1(pde_4m); 
    page_dir[1] = pde_4m;
    /* set the content of control regisster */
    flush_tlb(page_dir); 
}
/* flushed
 * description: reset cr3 content as the input page dir address(physical)
 * input: uint32_t* , page directory address
 */
void flush_tlb(uint32_t* page_dir) 
{
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
    : "eax", "memory");  
}
/*set_up_paging()
 * description: set up paging for process
 * input: int pid, which should be 0 or 1
 */
int set_up_paging(int pid)
{
    cli();//critical section start 
    uint32_t pde_4m_usr = 0; //initialize to all 0
    pde_4m_usr = set_present(pde_4m_usr); //set bit 0 present
    pde_4m_usr = set_page_size_1(pde_4m_usr); //set the page size 1, which is bit 7
    if (pid == 0)  pde_4m_usr = set_page_base_address_usr1(pde_4m_usr); // set the base address in physics memory for first program
    if (pid == 1)  pde_4m_usr = set_page_base_address_usr2(pde_4m_usr); // set the base address in physics memory for second program
    if (pid == 2)  pde_4m_usr = set_page_base_address_usr3(pde_4m_usr); // set the base address in physics memory for third program
    if (pid == 3)  pde_4m_usr = set_page_base_address_usr4(pde_4m_usr); // set the base address in physics memory for fourth program
    if (pid == 4)  pde_4m_usr = set_page_base_address_usr5(pde_4m_usr); // set the base address in physics memory for second program
    if (pid == 5)  pde_4m_usr = set_page_base_address_usr6(pde_4m_usr); // set the base address in physics memory for second program
    pde_4m_usr = set_read_write_1(pde_4m_usr); 
    pde_4m_usr = set_use_supervisor(pde_4m_usr); //set user supervisor 1 
    page_dir[32] = pde_4m_usr;  /* virtual memory is 128MB so index of page dir is 128MB / 4MB = 32*/
    /* set the content of control regisster */
    flush_tlb(page_dir);
    sti();//critical section ends
    return 0;
}

/*unmap_paging()
* description: unmap paging for a process
* input:none
* return: none
*/
void unmap_paging()
{
    cli();
    page_dir[33] = 0;
    flush_tlb(page_dir);
    sti();
}

/*set_user_video_map()
* description: set 132MB-136MB PTE 
* input: none
* ouput: none
*/
void set_user_video_map()
{
    int i;
    page_dir[33]=0;     /* 33 = (128MB + 4MB)/4MB       */
    page_dir[33]= 0x7 | (int32_t)page_vid_table;    //get the low three bits of the address
    for (i=0;i<1024;i++){                          //size of page table
        page_vid_table[i].val[0]=0;
    }
    page_vid_table[0].p_Base_address = VIDEO_MEMORY_ADDR >> PAGE_TABLE_RIGHT_OFF;   //the first 12 bits for video memory
    page_vid_table[0].Use_Supervisor = 1;
    page_vid_table[0].Present = 1;
    page_vid_table[0].Read_Write = 1;
    flush_tlb(page_dir);
}

/*map_video_page
 * description: map video pages for each terminal
 * input: starting address of video page and present of the page table
 * return: none 
 */
void map_video_page(int8_t* page_addr, int8_t* target_addr, uint32_t present) 
{
    int i =  (int)page_addr / PAGE_SIZE_4K;  /* find the page for video memory */
    page_table[i].p_Base_address = (int)target_addr >> PAGE_TABLE_RIGHT_OFF;   /* load the first 20 bits of video address */
    page_table[i].Present = present;  /* it present now */
    page_table[i].Read_Write = 1;   
    page_table[i].Use_Supervisor = 1;
    flush_tlb(page_dir);
}


/*set_vid_map
 * description: set vid_map when pit handler executed
 * input: starting address of video page and present of the page table
 * return: none 
 */
void set_vid_map(int8_t* target_addr, uint32_t present) 
{
    page_vid_table[0].p_Base_address = (int)target_addr >> PAGE_TABLE_RIGHT_OFF;   /* load the first 20 bits of video address */
    page_vid_table[0].Present = present;  /* it present now */
    page_table[0].Read_Write = 1;   
    page_table[0].Use_Supervisor = 1;
    flush_tlb(page_dir);
}
