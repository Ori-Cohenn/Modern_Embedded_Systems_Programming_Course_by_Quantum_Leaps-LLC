/*
 This lesson is about Automating the scheduling with round-robin policy on our MiROS.
 */
#include <stdint.h>
#include "miros.h"
#include "bsp.h"

uint32_t stack_blinky1[40];
OSThread blinky1;
void main_blinky1()
{
  while (1)
  {
    BSP_ledGreenOn();
    BSP_delay(BSP_TICKS_PER_SEC / 4U);
    BSP_ledGreenOff();
    BSP_delay(BSP_TICKS_PER_SEC * 3U / 4U);
  }
}

uint32_t stack_blinky2[40];
OSThread blinky2;
void main_blinky2()
{
  while (1)
  {
    BSP_ledBlueOn();
    BSP_delay(BSP_TICKS_PER_SEC / 2U);
    BSP_ledBlueOff();
    BSP_delay(BSP_TICKS_PER_SEC / 3U);
  }
}
uint32_t stack_blinky3[40];
OSThread blinky3;
void main_blinky3()
{
  while (1)
  {
    BSP_ledOrangeOn();
    BSP_delay(BSP_TICKS_PER_SEC / 3U);
    BSP_ledOrangeOff();
    BSP_delay(BSP_TICKS_PER_SEC *3U / 5U);
  }
}

int main(void)
{
  BSP_init();
  OS_init();

    /* start blinky1 thread */
  OSThread_start(&blinky1,
                 &main_blinky1,
                 stack_blinky1, sizeof(stack_blinky1));

    /* start blinky2 thread */
  OSThread_start(&blinky2,
                 &main_blinky2,
                 stack_blinky2, sizeof(stack_blinky2));
    /* start blinky3 thread */
  OSThread_start(&blinky3,
                 &main_blinky3,
                 stack_blinky3, sizeof(stack_blinky3));

    /* transfer control to the RTOS to run the threads */
    OS_run();
	
  // return 0;
}
/*
This lesson focuses on advancing the MiROS RTOS with a round-robin scheduling mechanism,
enabling efficient thread management and improving the system's design and execution.
Here's a detailed breakdown:

 1. Setting Up the Scheduler
The goal of round-robin scheduling is to automatically switch between multiple threads in a circular order.
To achieve this:
- `OS_thread[]` Array: A pre-allocated array stores pointers to active threads.
Its size is set to handle up to 32 threads, with room for future expansion.
- Counters and Indices:
  - `OS_threadNum` keeps track of the number of active threads.
  - `OS_currIdx` stores the current thread's index,
incremented in a circular fashion to switch to the next thread.
- Thread Registration: The `OSThread_start()` function registers new threads into the array and increments `OS_threadNum`.
The round-robin scheduler uses this array to pick the next thread.

Assertions (`Q_ASSERT()`) are introduced to ensure no overflow occurs when adding threads,
maintaining system stability.

 2. Automating Scheduling
Previously, thread scheduling was manual,
with hardcoded logic to switch between two threads (`blinky1` and `blinky2`).
The new round-robin design eliminates this by:
- Dynamically managing threads via `OS_thread[]`.
- Automating thread selection using `OS_currIdx` to pick the next thread in the array,
wrapping back to the start when reaching the last thread.

This composable design allows new threads (e.g., `blinky3`) to be added without modifying
the existing scheduler or RTOS code. Each thread runs independently and simultaneously,
improving scalability.

 3. Refactoring RTOS Initialization
The initialization process was restructured to ensure proper startup:
- Interrupts Configuration: Previously,
interrupts were enabled early in the `BSP_init()` function,
risking context switches before the RTOS was fully initialized.
This could lead to incomplete thread setup or skipped initialization steps.
- New API (`OS_run()`):
  - Moved interrupt configuration and enabling to a callback function,
  `OS_onStartup()`, called at the end of `main()`.
  - Replaces the endless `while(1)` loop, transferring control to the RTOS.

The result is a clean, structured, and safer startup process.


 4. Testing Performance
The scheduler and RTOS performance were evaluated using a logic analyzer and oscilloscope:
- Metrics Captured:
  - Time for context switch: ~10.8 microseconds (778 clock cycles).
  - RTOS overhead: 10.8*100/1e6=0.001% at a 72 MHz clock tick, or ~1% at a 1 kHz tick rate.
- Analysis Setup:
  - LEDs on the board toggled at high speeds to measure activity.
  - A dedicated test pin (`PA5`) was configured as a trigger for context switches,
  ensuring precise measurements.

The low overhead confirms the RTOS's efficiency, even with rapid context switches.


 5. Identified Inefficiencies
The `BSP_delay()` function, which relies on busy-waiting, was highlighted as a major inefficiency:
- Threads waste CPU cycles waiting for delays to expire.
- The lesson hints at implementing blocking, where delayed threads relinquish CPU control until ready,
drastically reducing wasted resources.


 6. Summary and Future Direction
The round-robin scheduler successfully automates thread switching, improves scalability,
and enhances initialization safety. With this foundation, the next step will address blocking,
making the RTOS more efficient and versatile by allowing threads to wait without consuming CPU time.
This sets the stage for handling more complex embedded systems.*/
