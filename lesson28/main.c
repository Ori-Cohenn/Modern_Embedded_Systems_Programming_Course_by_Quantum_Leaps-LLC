/*
 This lesson is about Mutual exclusion mechanisms.
 */

#include "qpc.h"
#include "bsp.h"

Q_DEFINE_THIS_FILE

QXSemaphore SW1_sema;


uint32_t stack_blinky1[40];
QXThread blinky1;
void main_blinky1(QXThread * const me)
{
	while (1){
        BSP_sendMorseCode(0xA8EEE2A0U); /* "SOS" the function shared between blinky1&3 */
        QXThread_delay(1U); /* block for 1 tick */
	}
}

uint32_t stack_blinky2[40];
QXThread blinky2;
void main_blinky2(QXThread * const me)
{
	while (1){
		uint32_t volatile i;

		QXSemaphore_wait(&SW1_sema,  /* pointer to semaphore to wait on */
						QXTHREAD_NO_TIMEOUT); /* timeout for waiting no timeout means forever */

		for (i = 3*1500U; i != 0U; --i) {
			//BSP_ledOrangeOn();
			//BSP_ledOrangeOff();
			BSP_ledOrangeToggle();
		}
	}
}
uint32_t stack_blinky3[40];
QXThread blinky3;
void main_blinky3(QXThread * const me)
{
  while (1)
  {
	BSP_sendMorseCode(0xE22A3800U); /* "TEST" */
	BSP_sendMorseCode(0xE22A3800U); /* "TEST" */
	QXThread_delay(5U);
  }
}


int main(void)
{
	QF_init();
	BSP_init();
    /* initialize the SW1_sema semaphore as binary, signaling semaphore */
    QXSemaphore_init(&SW1_sema, /* pointer to semaphore to initialize */
                     0U,  /* initial semaphore count (singaling semaphore) */
                     1U); /* maximum semaphore count (binary semaphore) */

    /* initialize and start blinky1 thread */
    QXThread_ctor(&blinky1, &main_blinky1, 0);
    QXTHREAD_START(&blinky1,
                   5U, /* priority */
                   (void *)0, 0, /* message queue (not used) */
                   stack_blinky1, sizeof(stack_blinky1), /* stack */
                   (void *)0); /* extra parameter (not used) */

    /* initialize and start blinky2 thread */
    QXThread_ctor(&blinky2, &main_blinky2, 0);
    QXTHREAD_START(&blinky2,
                   2U, /* priority */
                   (void *)0, 0, /* message queue (not used) */
                   stack_blinky2, sizeof(stack_blinky2), /* stack */
                   (void *)0); /* extra parameter (not used) */

    /* initialize and start blinky3 thread */
    QXThread_ctor(&blinky3, &main_blinky3, 0);
    QXTHREAD_START(&blinky3,
                   1U, /* priority */
                   (void *)0, 0, /* message queue (not used) */
                   stack_blinky3, sizeof(stack_blinky3), /* stack */
                   (void *)0); /* extra parameter (not used) */

				   
    /* transfer control to the RTOS to run the threads */
    return QF_run();
}

/*
The Modern Embedded Systems Programming course's seventh lesson introduces mechanisms for protecting shared resources among concurrent threads in RTOS.  
The lesson emphasizes mutual exclusion mechanisms to prevent conflicts when threads access shared variables, functions, or hardware resources.  

Threads in a RTOS share the same address space, unlike processes in desktop OSes like Windows or Linux.  
This allows threads to share memory and registers, but it also introduces risks like race conditions.  
For example, toggling LEDs using the same GPIO register without protection can lead to incorrect LED states.  

The first solution discussed is a critical section, which disables interrupts during access to shared resources.  
Critical sections work for short durations but can increase interrupt latency.  
Unoptimized critical sections may also fail in nested cases, re-enabling interrupts prematurely.  
RTOS-specific critical sections, like in QXK, selectively disable interrupts and support nesting.  

For longer resource-sharing durations, critical sections are insufficient.  
A function sending Morse code via LED blinking introduces prolonged shared access.  
Semaphores ensure mutual exclusion but may cause unbounded priority inversion.  
This occurs when a low-priority thread holding a semaphore blocks higher-priority threads indefinitely.  

Two advanced mechanisms address this issue.  
1. Selective Scheduler Locking prevents scheduling threads below a priority ceiling.  
   Higher-priority threads remain unaffected, and priority inversion is avoided.  
   It is non-blocking and preserves interrupt latency.  
2. Priority-Ceiling Mutex promotes a thread holding the mutex to a predefined ceiling priority.  
   This blocks lower-priority threads from preempting the mutex holder.  
   It allows blocking operations, unlike scheduler locking.  

Priority inheritance is another mutex mechanism, but it has drawbacks.  
It increases context switches and delays high-priority threads, risking deadline misses.  
Priority-ceiling mutexes are simpler and preferred for hard real-time systems.  

The lesson concludes by explaining that shared-state concurrency has dominated since the 1980s.  
However, modern architectures that avoid shared resources eliminate the need for mutual exclusion mechanisms.  
Future lessons will explore these approaches and object-oriented programming.
*/