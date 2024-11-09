/*
 This lesson is about Automating the context switch, creating MiROS -> 
 Minimal Real-time Operating system from scratch!
 
 */
#include <stdint.h>
#include "miros.h"
#include "bsp.h"

uint32_t stack_blinky1[40];
OSThread blinky1;
void main_blinky1() {
    while (1) {
        BSP_ledGreenOn();
        BSP_delay(BSP_TICKS_PER_SEC / 4U);
        BSP_ledGreenOff();
        BSP_delay(BSP_TICKS_PER_SEC * 3U / 4U);
    }
}

uint32_t stack_blinky2[40];
OSThread blinky2;
void main_blinky2() {
    while (1) {
        BSP_ledBlueOn();
        BSP_delay(BSP_TICKS_PER_SEC / 2U);
        BSP_ledBlueOff();
        BSP_delay(BSP_TICKS_PER_SEC / 3U);
    }
}

int main(void) {
    BSP_init();
    OS_init();

    /* fabricate Cortex-M ISR stack frame for blinky1 */
    OSThread_start(&blinky1,
                   &main_blinky1,
                   stack_blinky1, sizeof(stack_blinky1));

    /* fabricate Cortex-M ISR stack frame for blinky2 */
    OSThread_start(&blinky2,
                   &main_blinky2,
                   stack_blinky2, sizeof(stack_blinky2));

    //BSP_ledRedOn();
    while (1) {
    }

    //return 0;
}
/*
Here's a more detailed summary:

This is Lesson 23 of the Modern Embedded Systems Programming course, focusing on building a minimal Real-Time Operating System (RTOS) and automating context switching. Here's a comprehensive breakdown:

1. Background and Context
- Continues from Lesson 22 where manual context switching was explored
- Addresses the challenge of running multiple background loops independently
- Uses example of blinking two LEDs (green and blue) independently
- Introduces RTOS as a solution to extend foreground/background architecture

2. RTOS Development Approach
- Creates a minimal RTOS (MiROS) from scratch for better learning experience
- Avoids complexity of studying existing systems like FreeRTOS
- Focuses on fundamental concepts and building blocks
- Uses hands-on, practical implementation

3. Implementation Structure
- Created two main files:
  * miros.h (API definitions)
  * miros.c (implementation)
- Included proper documentation, licensing (GPL), and copyright information
- Established clear naming conventions with "OS" prefix

4. Key Components
a) Thread Management:
   - Defined OSThread struct (Thread Control Block)
   - Implemented stack pointer management
   - Created thread creation function (OSThread_start)
   - Handled proper stack alignment (8-byte boundary)

b) Context Switching:
   - Used PendSV exception for context switching
   - Implemented proper interrupt priority management
   - Set PendSV to lowest priority (0xF0 in STM)
   - Set SysTick to higher priority (0)

c) Scheduling:
   - Implemented OS_sched() function
   - Created OS_curr and OS_next pointers for thread tracking
   - Handled critical sections properly
   - Prepared for future scheduling strategies

5. Technical Implementation Details
a) Assembly Code Integration:
   - Implemented PendSV_Handler in assembly
   - Handled register saving/restoration (R4-R11)
   - Managed stack pointer operations
   - Implemented proper interrupt enabling/disabling for Critical section

b) Memory Management:
   - Handled stack initialization
   - Implemented proper memory alignment 8 bytes 
   - Added stack pattern filling (0xDEADBEEF) for debugging
   - Managed thread context storage


6. Performance Considerations
- Utilized ARM Cortex-M's tail-chaining optimization
- Minimized context switching overhead
- Handled race conditions through proper interrupt management
- Implemented efficient register saving/restoration

7. Safety and Reliability Features
- Proper interrupt priority management
- Critical section handling
- Stack alignment enforcement
- Memory pattern filling for debugging
- Race condition prevention by using Critical section

8. Future Enhancements
- Implementation of automatic scheduling
- Addition of round-robin scheduling policy
- Development of complete time-sharing system
- Potential optimization of context switching code

9. Educational Value
- Provides hands-on experience with RTOS concepts
- Demonstrates practical implementation of theoretical concepts
- Offers deep understanding of context switching
- Shows real-world debugging techniques

10. Key Achievements
- Successfully implemented automated context switching
- Created foundation for a functional RTOS
- Demonstrated working multi-threading
- Established framework for future enhancements

This lesson serves as a crucial stepping stone in understanding RTOS implementation, 
providing both theoretical knowledge and practical experience in embedded systems programming.
The next lesson will build upon this foundation to implement automated scheduling using a round-robin policy.
*/