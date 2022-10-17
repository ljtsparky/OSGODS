## BUG 1

**Expected Result：** Press q on the keyboard, display single q on the screen

**Actual Result：** Press q on the keyboard, display q and a number on the screen

**Explanation；** When press q and loose q, the linux get two signal, so to display q, we have to mask the second signal.

## BUG 2

bug: The screen is static after executing the test_interrupt function but not alternating

solution: Set the frequency of RTC from 1024Hz to 2Hz

## BUG 3

bug: RTC can't get another interrupt after handling one interrupt

solution: Read the RTC status register C at the end of each handler to make sure that RTC can receive other interrupts

## BUG 4
2022 Oct 15 Li jiatong: paging init, when we initialize the page dir entry , there are two different entries to be entered, so we need to treat them differently. 


we solved the bug by setting the page size of the second entry of page dir to be 1 meaning that it corresponds to a 4MB page space, and then make the address of the 4MB space 


which is 0x400000 to right shift 12 because 12 is exactly 4MB, as a result we get 1 to put into the base_address part.


## BUG 5
2022 Oct 15 Li Jiatong: about pic initialization: we found it hard to know whether we initialized those successfully, so used some printf to indicate we finished the init
