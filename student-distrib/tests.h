#ifndef TESTS_H
#define TESTS_H


#define BEFORE_KERNEL_PAGE 0x312345 
#define ONE_AFTER_KERNEL_PAGE 0x512343
#define END_OF_KERNEL_PAGE 0x4FFFFF
#define START_OF_KERNEL_PAGE 0x400000
#define START_OF_VIDEO_MEM 0xB8000
#define BEFORE_VIDEO_MEM 0xB7FFF 
#define END_OF_VIDEO_MEM 0xB8FFF
#define AFTER_VIDEO_MEM 0xB9000
// test launcher
void launch_tests();
// enum file_table{

// }

#endif /* TESTS_H */
