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

extern void init_paging ();
#endif
