#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "rtc.h"
#include "terminal.h"
#include "keyboard.h"
#include "filesystem.h"

#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER \
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result) \
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure()
{
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}

/* Checkpoint 1 tests */

/* IDT Test - Example
 *
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test()
{
	TEST_HEADER;
	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i)
	{
		if ((idt[i].offset_15_00 == NULL) &&
			(idt[i].offset_31_16 == NULL))
		{
			assertion_failure();
			result = FAIL;
		}
	}
	return result;
}

// divide by zero test
// should cause divide by zero exception
uint8_t der;

// video_mem_test_start
// description: test the paging
// input: none output:none return:PASS if dereferencing suceeded
// side effect:none
int video_mem_test_start()
{
	TEST_HEADER;
	der = *(uint8_t *)(START_OF_VIDEO_MEM);
	// uint8_t *page_entry_pt = (uint8_t *)(START_OF_VIDEO_MEM);
	// der = *page_entry_pt;
	return PASS;
}
// video_mem_test_end
// description: test the paging
// input: none output:none return:PASS if dereferencing suceeded
// side effect:none
int video_mem_test_end()
{
	TEST_HEADER;
	der = *(uint8_t *)(END_OF_VIDEO_MEM);
	// uint8_t *page_entry_pt = (uint8_t *)(END_OF_VIDEO_MEM);
	// der = *page_entry_pt;
	return PASS;
}
// kernel_page_start
// description: test the paging
// input: none output:none return:PASS if dereferencing suceeded
// side effect:none
int kernel_page_start()
{
	TEST_HEADER;
	der = *(uint8_t *)(START_OF_KERNEL_PAGE);
	// uint8_t *page_entry_pt = (uint8_t *)(START_OF_KERNEL_PAGE);
	// der = *page_entry_pt;
	return PASS;
}
// kernel_page_end
// description: test the paging
// input: none output:none return:PASS if dereferencing suceeded
// side effect:none
int kernel_page_end()
{
	TEST_HEADER;
	der = *(uint8_t *)(END_OF_KERNEL_PAGE);
	// uint8_t *page_entry_pt = (uint8_t *)(END_OF_KERNEL_PAGE);
	// der = *page_entry_pt;
	return PASS;
}
// between_kernel_video_mem_test
// description: test the paging
// input: none output:none return:PASS if dereferencing suceeded
// side effect:none
int between_kernel_video_mem_test()
{
	TEST_HEADER;
	der = *(uint8_t *)(BEFORE_KERNEL_PAGE);
	// uint8_t *page_entry_pt = (uint8_t *)(BEFORE_KERNEL_PAGE);
	// der = *page_entry_pt;
	return PASS;
}
// after_kernel_page_test
// description: test the paging
// input: none output:none return:PASS if dereferencing suceeded
// side effect:none
int after_kernel_page_test()
{
	TEST_HEADER;
	der = *(uint8_t *)(ONE_AFTER_KERNEL_PAGE);
	// uint8_t *page_entry_pt = (uint8_t *)(ONE_AFTER_KERNEL_PAGE);
	// der = *page_entry_pt;
	return PASS;
}
// before_video_memory
// description: test the paging
// input: none output:none return:PASS if dereferencing suceeded
// side effect:none
int before_video_memory()
{
	TEST_HEADER;
	der = *(uint8_t *)(BEFORE_VIDEO_MEM);
	// uint8_t *page_entry_pt = (uint8_t *)(BEFORE_VIDEO_MEM);
	// der = *page_entry_pt;
	return PASS;
}

// SIMD_floating_point_test
// description: test the paging
// input: none output:none return:PASS if dereferencing suceeded
// side effect:none
int SIMD_floating_point_test(void)
{
	TEST_HEADER;
	asm volatile("int $19");
	return PASS;
}

/* check point 2 test */

/*test_terminal_write
 * input: none
 * description: test the number of bits terminal_write writes
 */
int test_terminal_write()
{
	TEST_HEADER;
	uint8_t test1[33] = "MP3 CHECK POINT 2 WRITE TEST!!!\n"; /* 33 is the length of the sentence */
	if (terminal_write(0, test1, 33) == 33)
		return PASS;
	else
		return FAIL;
}

// test_terminal_read
// test the function
// input: none  output: some msg necessary to denote where we are
// return: PASS   side effect: changing the test1 buffer content, but realeased
int test_terminal_read()
{
	TEST_HEADER;
	char test1[128];	/* 128 is max number of char in keyboard buffer */
	int i;
	/* test the case when enter is not pressed */
	for (i = 0; i < 2; i++)
	{
		printf("Terminal read test ROUND %d begin\n", i);
		int nbytes_success = terminal_read(0, test1, KEYB_BUFF_LEN - i);
		printf("input_bytes: %d\n", nbytes_success);
		printf("Terminal read test ROUND %d end\n", i);
	}
	/* test the case when enter is pressed */
	for (i = 0; i < 5; i++)
	{
		set_read_signal_enable();
		printf("Terminal read test ROUND %d begin\n", i + 2);
		int nbytes_success = terminal_read(0, test1, KEYB_BUFF_LEN + 2 - i);
		printf("input_bytes: %d\n", nbytes_success);
		printf("Terminal read test ROUND %d end\n", i + 2);
	}
	return PASS;
}

/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */
void launch_tests()
{
	// TEST_OUTPUT("test rtc",test_rtc());
	// TEST_OUTPUT("idt_test", idt_test());
	// TEST_OUTPUT("video_mem_test_start", video_mem_test_start());
	// TEST_OUTPUT("video_mem_test_end", video_mem_test_end());
	// TEST_OUTPUT("kernel_page_start", kernel_page_start());
	// TEST_OUTPUT("kernel_page_end", kernel_page_end());
	// TEST_OUTPUT("between_kernel_video_mem_test",between_kernel_video_mem_test());
	// // TEST_OUTPUT("after_kernel_page_test",after_kernel_page_test());
	// // TEST_OUTPUT("before_video_memory",before_video_memory());
	// TEST_OUTPUT("divide error test", Divide_test());

	/* CHECK POINT 2*/
	// clear();
	// TEST_OUTPUT("write test", test_terminal_write());
	// delay(100);
	// clear();
	// TEST_OUTPUT("read test", test_terminal_read());
	// delay(100);
	// clear();
	// TEST_OUTPUT("test file",file_test_1());
	// delay(100);
	// clear();
	// TEST_OUTPUT("test file",file_test_2(10));
	// delay(100);
	// clear();
	// TEST_OUTPUT("test file", file_test_3(5));
	// delay(100);
	// clear();
	// TEST_OUTPUT("test file", file_test_3(4));
	// delay(100);
	// clear();
	// delay(100);
	// clear();
	// TEST_OUTPUT("test file", file_test_111(2));
	// delay(100);
	// clear();
	// TEST_OUTPUT("test file", file_test_111(1));
	// delay(100);
	// // launch your tests here
}
