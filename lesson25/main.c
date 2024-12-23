/*
 This lesson is about Efficient blocking of threads on our MiROS.
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
    OS_delay(BSP_TICKS_PER_SEC / 4U);
    BSP_ledGreenOff();
    OS_delay(BSP_TICKS_PER_SEC * 3U / 4U);
  }
}

uint32_t stack_blinky2[40];
OSThread blinky2;
void main_blinky2()
{
  while (1)
  {
    BSP_ledOrangeOn();
    OS_delay(BSP_TICKS_PER_SEC / 2U);
    BSP_ledOrangeOff();
    OS_delay(BSP_TICKS_PER_SEC / 3U);
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
  OSThread_start(&blinky1,
                 &main_blinky1,
                 stack_blinky1, sizeof(stack_blinky1));

    /* start blinky2 thread */
  OSThread_start(&blinky2,
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
Miro Samek introduces the concept of replacing inefficient polling delays with blocking delays in the MiROS RTOS.  
The lesson begins by addressing the current use of polling in the `BSP_delay()` function, 
which wastes CPU cycles, and presents a more efficient method using context switching.  
With blocking delays, a thread will be suspended and not consume any CPU time during its delay period,
 allowing other threads to run instead.  
The lesson walks through the modification of the RTOS to support blocking.  
It explains how a thread can transition from a Running state to a Blocked state and how the idle thread is always available to run when no other threads are ready.  
The lesson also introduces the concept of a timeout counter for each thread, which is updated periodically by the OS_tick() function.  
Threads are unblocked when their timeout counters reach zero.  
The course further explores the thread lifecycle, which includes states such as Dormant,
 Preempted, Running, and Blocked, and how these states interact with the RTOS scheduler.  
A key part of the implementation is the OS_readySet bitmask, which efficiently tracks ready threads and avoids scheduling blocked threads.  
The lesson concludes with the implementation of OS_delay(), a function that uses the new blocking mechanism,
 and highlights its positive impact on CPU usage and system performance.  
The addition of the idle thread ensures the system remains efficient even when other threads are blocked, contributing to a more responsive and energy-efficient system.  
*/