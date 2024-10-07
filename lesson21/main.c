/*
 This lesson is about the ubiquitous foreground-background architecture, also known as the super-loop or main+interrupts.
 is an important stepping stone to understanding a Real-Time Operating System (RTOS)
 The course explains two approaches to the foreground-background architecture: 
 a blocking, sequential method (which waits for events in polling loops) and a more flexible,
 non-blocking, *event-driven* approach. While the non-blocking method is more complex,
 it enhances responsiveness by continuously handling events in real time. 
 */
#include <stdint.h>
#include "bsp.h"


#if 0 //making it inactive
/* background code: sequential with blocking version */
int main(void) {
    BSP_init();
    while (1) {
        BSP_ledGreenOn();
        BSP_delay(BSP_TICKS_PER_SEC / 4U);
        BSP_ledGreenOff();
        BSP_delay(BSP_TICKS_PER_SEC * 3U / 4U);
    }

    //return 0;
}
#endif

/* background code: non-blocking version */
int main() {
	BSP_init();
	while (1) {
			/* Blinky polling state machine */
			static enum {
					INITIAL,
					OFF_STATE,
					ON_STATE
			} state = INITIAL;
			static uint32_t start;
			switch (state) {
					case INITIAL:
							start = BSP_tickCtr();
							state = OFF_STATE; /* initial transition */
							break;
					case OFF_STATE:
							if ((BSP_tickCtr() - start) > BSP_TICKS_PER_SEC * 3U / 4U) {
									BSP_ledGreenOn();
									start = BSP_tickCtr();
									state = ON_STATE; /* state transition */
							}
							break;
					case ON_STATE:
							if ((BSP_tickCtr() - start) > BSP_TICKS_PER_SEC / 4U) {
									BSP_ledGreenOff();
									start = BSP_tickCtr();
									state = OFF_STATE; /* state transition */
							}
							break;
					default:
							//error();
							break;
			}
    }
	return 0;
}

/*
In the foreground-background architecture, embedded systems divide tasks into two parts:

Background: This is an endless loop that runs in the main program. 
It handles lower-priority tasks and keeps running continuously unless interrupted. 
It doesn't work on strict timing, so it's good for tasks that don't need precise timing or real-time responses.

Foreground: This part is handled by interrupts.
When an important event (like a timer or sensor signal) happens, the foreground interrupts the background and takes over briefly to handle the event.
After the event is processed, control goes back to the background.

This approach allows the system to respond quickly to important events while still running less urgent tasks in the background. 
However, communication between the two parts needs careful handling to avoid conflicts, like race conditions.
*/
