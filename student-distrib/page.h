#ifndef _PAGE_H
#define _PAGE_H
#define set_page_base_address_1(x) ( (x) | (1L << 22 ) )
#define set_present(x) ( (x) | 0x00000001 )
#define set_page_size_1(x) ( (x) | 0x00000080 )
#define set_read_write_1(x) ( (x) | 0x00000002 )
#define set_use_supervisor(x) ( (x) | 0x4)
#define NUM_PTE 1024
#define PAGE_SIZE_4K   4096
#define PAGE_TABLE_RIGHT_OFF 12
#define VIDEO_MEMORY_ADDR 0XB8000
#define VIDEO_SIZE 2000

#define set_page_base_address_usr1(x) ( (x) | (0x800000 ) )  /* first 22 bits of 8Mb, the shell */
#define set_page_base_address_usr2(x) ( (x) | (0xC00000 ) )  /* first 22 bits of 12Mb */
#define set_page_base_address_usr3(x) ( (x) | (0x1000000 ) )  /* first 22 bits of 16Mb */
#define set_page_base_address_usr4(x) ( (x) | (0x5000000 ) )  /* first 22 bits of 8Mb, the shell */
#define set_page_base_address_usr5(x) ( (x) | (0x9000000 ) )  /* first 22 bits of 12Mb */
#define set_page_base_address_usr6(x) ( (x) | (0xD000000 ) )  /* first 22 bits of 16Mb */

extern void init_paging ();
extern void flush_tlb(unsigned int* page_dir);
void set_user_video_map();
int set_up_paging(int pid);
void unmap_paging();

#endif
