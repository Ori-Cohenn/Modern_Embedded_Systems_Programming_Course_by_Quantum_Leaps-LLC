/*
 This lesson is about What is "real-time"? Preemptive, priority-based scheduling.
 CLZ= count leading zeros
 */
#include <stdint.h>
#include "miros.h"
#include "bsp.h"

uint32_t stack_blinky1[40];
OSThread blinky1;
void main_blinky1()
{
	while (1){
		uint32_t volatile i;
		for (i = 1500U; i != 0U; --i) {
			BSP_ledGreenOn();
			BSP_ledGreenOff();
		}
		OS_delay(1U); /* block for 1 tick */
	}
}

uint32_t stack_blinky2[40];
OSThread blinky2;
void main_blinky2()
{
  while (1)
  {
        uint32_t volatile i;
        for (i = 3*1500U; i != 0U; --i) {
            BSP_ledOrangeOn();
            BSP_ledOrangeOff();
        }
        OS_delay(50U); /* block for 50 ticks */
    }
}
uint32_t stack_blinky3[40];
OSThread blinky3;
void main_blinky3()
{
  while (1)
  {
    BSP_ledRedOn();
    OS_delay(BSP_TICKS_PER_SEC / 3U);
    BSP_ledRedOff();
    OS_delay(BSP_TICKS_PER_SEC *3U / 5U);
  }
}

uint32_t stack_idleThread[40];


int main(void)
{
	BSP_init();
	OS_init(stack_idleThread, sizeof(stack_idleThread));

	/* start blinky1 thread */
	OSThread_start(	&blinky1,
					5U, /* priority */
					&main_blinky1,
					stack_blinky1, sizeof(stack_blinky1));


	/* start blinky2 thread */
	OSThread_start(&blinky2,
					2U, /* priority lower than blinky1*/
					 &main_blinky2,
					 stack_blinky2, sizeof(stack_blinky2));

	/* start blinky3 thread */
	//  OSThread_start(&blinky3,
	//                 &main_blinky3,
	//                 stack_blinky3, sizeof(stack_blinky3));

	/* transfer control to the RTOS to run the threads */
	OS_run();

	// return 0;
}

/*
In this lesson of the *Modern Embedded Systems Programming* course, 
Miro Samek introduces real-time computing concepts and implements a preemptive,
priority-based scheduler in the MiROS RTOS.  
The lesson begins by explaining the importance of real-time requirements,
where computations must meet strict deadlines to be useful, particularly in hard real-time systems, 
like deploying airbags or controlling spacecraft descent. Unlike the previous round-robin scheduler,
which focused on fairness,
the new scheduler prioritizes threads based on their importance to ensure critical deadlines are met.  
A priority-based scheduler assigns each thread a unique priority at startup. 
The scheduler always runs the highest-priority thread that is ready-to-run,
preempting lower-priority threads when necessary. This implementation uses a direct priority numbering scheme, 
where higher numbers indicate higher priorities.  
The RTOS is modified to include a bitmask (`OS_readySet`) to efficiently track ready threads and find the highest-priority thread using the Count Leading Zeros (CLZ) instruction. 
The scheduler leverages this operation to quickly determine the next thread to execute without scanning all threads, improving performance.  
Thread blocking, introduced in the previous lesson, is essential for priority-based scheduling,
as it ensures that lower-priority threads can run only when higher-priority threads voluntarily relinquish the CPU.
Without blocking, lower-priority threads would be entirely starved of CPU time.  
The lesson also introduces Rate-Monotonic Analysis (RMA),
a mathematical method for assigning priorities based on thread rates and ensuring all threads meet their deadlines if the total CPU utilization is below a specific bound (e.g., 70%). 
This approach decouples threads in the time domain, allowing high-priority threads to remain unaffected by changes in lower-priority thread behavior.  

By implementing the priority-based scheduler, 
the MiROS RTOS now supports preemptive real-time scheduling, a standard in modern RTOS design.
The next lesson will explore inter-thread synchronization and communication mechanisms introduced in the 1980s.  
*/