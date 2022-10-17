#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "rtc.h"

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
	der= *(uint8_t *)(START_OF_VIDEO_MEM);
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
	der= *(uint8_t *)(END_OF_VIDEO_MEM);
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
	der= *(uint8_t *)(START_OF_KERNEL_PAGE);
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
	der= *(uint8_t *)(END_OF_KERNEL_PAGE);
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
	der= *(uint8_t *)(BEFORE_KERNEL_PAGE);
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
	der= *(uint8_t *)(ONE_AFTER_KERNEL_PAGE);
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
	der= *(uint8_t *)(BEFORE_VIDEO_MEM);
	// uint8_t *page_entry_pt = (uint8_t *)(BEFORE_VIDEO_MEM); 
	// der = *page_entry_pt;
	return PASS;
}
// Divide_test
// description: test the paging
// input: none output:none return:PASS if dereferencing suceeded
// side effect:none
int Divide_test(void)
{
	TEST_HEADER;
	int b = 1 / 0; /* devide zero error */
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

/* Test suite entry point */
void launch_tests()
{
	TEST_OUTPUT("idt_test", idt_test());
	TEST_OUTPUT("video_mem_test_start", video_mem_test_start());
	TEST_OUTPUT("video_mem_test_end", video_mem_test_end());
	TEST_OUTPUT("kernel_page_start", kernel_page_start());
	TEST_OUTPUT("kernel_page_end", kernel_page_end());
	TEST_OUTPUT("between_kernel_video_mem_test",between_kernel_video_mem_test());
	// TEST_OUTPUT("after_kernel_page_test",after_kernel_page_test());
	// TEST_OUTPUT("before_video_memory",before_video_memory());
	TEST_OUTPUT("divide error test", Divide_test());
	// launch your tests here
}

// add more tests here

/* Checkpoint 2 tests */
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */
