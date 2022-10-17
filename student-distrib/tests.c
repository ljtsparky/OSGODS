#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "rtc.h"


#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}
static inline void dividezero_exception(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $0");
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
int idt_test(){
	TEST_HEADER;
	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}
// int keyboard_test(){
// 	TEST_HEADER;
// 	int i;
// 	int result=PASS;
// 	for (i=0;i<10;++i){
		
// 	}
// }
// int rtc_test(){
// 	TEST_HEADER;
// 	int i;
// 	int result = PASS;
//     init_rtc();
//     rtc_intr_handler();
// }

// divide by zero test
// should cause divide by zero exception
int Divide_test(void)
{
	TEST_HEADER;
	printf("divide test\n");
	// dividezero_exception();
	return PASS;
}

/* Test suite entry point */
void launch_tests(){
	TEST_OUTPUT("idt_test", idt_test());
	TEST_OUTPUT("divide zero test",Divide_test());
	printf("end of lauchwn test\n");
	// TEST_OUTPUT("keyboard test",keyboard_test());
	// TEST_OUTPUT("real time clock test",rtc_test());
	// TEST_OUTPUT("interrupt test",interrupt_test());
	// launch your tests here
}


// add more tests here

/* Checkpoint 2 tests */
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */
