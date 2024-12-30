/*
 This lesson is about Synchronization and communication among concurrent threads.
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
		uint32_t volatile i;
		for (i = 1500U; i != 0U; --i) {
			BSP_ledGreenOn();
			BSP_ledGreenOff();
		}
		QXThread_delay(1U); /* block for 1 tick */
	}
}

uint32_t stack_blinky2[40];
QXThread blinky2;
void main_blinky2(QXThread * const me)
{
  while (1)
  {
        uint32_t volatile i;
	  
		QXSemaphore_wait(&SW1_sema,  /* pointer to semaphore to wait on */
						 QXTHREAD_NO_TIMEOUT); /* timeout for waiting no timeout means forever */
       
	  for (i = 3*1500U; i != 0U; --i) {
            BSP_ledOrangeOn();
            BSP_ledOrangeOff();
        }
    }
}
uint32_t stack_blinky3[40];
QXThread blinky3;
void main_blinky3(QXThread * const me)
{
  while (1)
  {
    BSP_ledRedOn();
    QXThread_delay(BSP_TICKS_PER_SEC / 3U);
    BSP_ledRedOff();
    QXThread_delay(BSP_TICKS_PER_SEC *3U / 5U);
  }
}


int main(void)
{
	BSP_init();
	QF_init();

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
    //QXThread_ctor(&blinky3, &main_blinky3, 0);
    //QXTHREAD_START(&blinky3,
    //               1U, /* priority */
    //               (void *)0, 0, /* message queue (not used) */
    //               stack_blinky3, sizeof(stack_blinky3), /* stack */
    //               (void *)0); /* extra parameter (not used) */

				   
    /* transfer control to the RTOS to run the threads */
    return QF_run();
}

/*
In this lesson of the *Modern Embedded Systems Programming* course,
Miro Samek introduces inter-thread synchronization using semaphores and demonstrates how to port an application from the toy MiROS RTOS
to the professional-grade QXK RTOS in the QP/C framework.  

The lesson begins by addressing the limitations of MiROS RTOS in implementing synchronization mechanisms, such as semaphores.  

Semaphores, originally conceptualized by Edsger Dijkstra,
are introduced as tools for signaling between threads,
akin to railroad signals ensuring safe and orderly operation.  

Porting the application involves replacing MiROS-specific APIs with QXK equivalents,
such as `QXThread` and `QXSemaphore`, while leveraging the QXK ISR macros (`QXK_ISR_ENTRY` and `QXK_ISR_EXIT`) for interrupt handling.  

Special care is taken to configure interrupt priorities correctly using the ARM Cortex-M BASEPRI register,
distinguishing between kernel-aware and kernel-unaware interrupts to optimize interrupt latency.  

A binary semaphore is used to synchronize the `blinky2` thread with button presses detected via a GPIO-A interrupt.  

The ISR signals the semaphore when the button is pressed,
unblocking the `blinky2` thread to toggle the Orange LED.  

However, mechanical switch debouncing is highlighted as a critical challenge,
leading to multiple false triggers.  

Through logic analyzer traces,
the lesson explores how the QXK kernel ensures priority-based preemption and how semaphore tokens are exchanged under different scenarios.  

For example, when a high-priority thread like `blinky1` is running,
the lower-priority `blinky2` thread must wait until it blocks, 
illustrating the real-time scheduling principles of QXK.  

The lesson concludes with a deeper understanding of semaphores
as the foundation for inter-thread synchronization 
and sets the stage for discussing mutual exclusion in the next lesson.   
*/