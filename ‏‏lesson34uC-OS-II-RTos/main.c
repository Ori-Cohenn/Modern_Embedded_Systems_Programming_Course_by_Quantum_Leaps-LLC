/*
This lesson is about Event-Driven Programming, Best practices for concurrency & Active Object pattern
 */

#include "os.h" 	  /* uC/OS-II API, port and compile-time configuration */
#include "qassert.h"  /* embedded-system-friendly assertions */
#include "bsp.h"	  /* Board Support Package */

Q_DEFINE_THIS_MODULE("main") /* this module name for Q_ASSERT() */

/* The Blinky thread =========================================================*/
OS_STK stack_blinky[100]; /* task stack */

enum { INITIAL_BLINK_TIME = (OS_TICKS_PER_SEC / 4) };

/* data shared between tasks */
INT32U volatile shared_blink_time = INITIAL_BLINK_TIME;
OS_EVENT *shared_blink_time_mutex;

void main_blinky(void *pdata) { /* task function */
    (void)pdata; /* unused parameter(s) */

    while (1) { /* endless "superloop" */
        INT8U err;
        INT32U bt; /* local copy of shared_blink_time */

        OSMutexPend(shared_blink_time_mutex, 0, &err); /* mutual exclusion */
        Q_ASSERT(err == 0);
        bt = shared_blink_time;
        OSMutexPost(shared_blink_time_mutex); /* mutual exclusion */

        BSP_ledGreenOn();
        OSTimeDly(bt);       /* BLOCKING! */
        BSP_ledGreenOff();
        OSTimeDly(bt * 3U);  /* BLOCKING! */
    }
}

/* The Button thread =========================================================*/
OS_STK stack_button[100]; /* task stack */

void main_button(void *pdata) { /* task function */
    (void)pdata; /* unused parameter(s) */

    while (1) { /* endless "superloop" */
        INT8U err; /* uC/OS-II error status */

        /* wait on the button-press semaphore (BLOCK indefinitely) */
        OSSemPend(BSP_semaPress, 0, &err); /* BLOCKING! */
        Q_ASSERT(err == 0);
        BSP_ledBlueOn();

        /* update the blink time for the 'blink' thread */
        OSMutexPend(shared_blink_time_mutex, 0, &err); /* mutual exclusion */
        Q_ASSERT(err == 0);
        shared_blink_time >>= 1; /* shorten the blink time by factor of 2 */
        if (shared_blink_time == 0U) {
            shared_blink_time = INITIAL_BLINK_TIME;
        }
        OSMutexPost(shared_blink_time_mutex); /* mutual exclusion */

        /* wait on the button-release semaphore (BLOCK indefinitely) */
        OSSemPend(BSP_semaRelease, 0, &err); /* BLOCKING! */
        BSP_ledBlueOff();
    }
}

OS_EVENT *BSP_semaPress;   /* global semaphore handle */
OS_EVENT *BSP_semaRelease; /* global semaphore handle */

/* the main function =========================================================*/
int main() {
    INT8U err;

    BSP_init(); /* initialize the BSP */
    OSInit();   /* initialize uC/OS-II */

    /* initialize the RTOS objects before using them */
    BSP_semaPress   = OSSemCreate(0);
    Q_ASSERT(BSP_semaPress);
    BSP_semaRelease = OSSemCreate(0);
    Q_ASSERT(BSP_semaRelease);
    shared_blink_time_mutex = OSMutexCreate(OS_LOWEST_PRIO - 5, &err);
    Q_ASSERT(err == 0);

    /* create uC/OS-II task, see NOTE1 */
    err = OSTaskCreateExt(&main_blinky, /* the task function */
          (void *)0,      /* the 'pdata' parameter (not used) */
          &stack_blinky[(sizeof(stack_blinky) / sizeof(OS_STK)) - 1], /* ptos */
          OS_LOWEST_PRIO - 4, /* uC/OS-II task priority */
          OS_LOWEST_PRIO - 4, /* unique priority is used as the task ID */
          stack_blinky,   /* pbos */
          (INT32U)(sizeof(stack_blinky)/sizeof(OS_STK)),/* stack depth */
          (void *)0,      /* pext */
          (INT16U)0);     /* task options */
    Q_ASSERT(err == 0);

    /* create uC/OS-II task, see NOTE1 */
    err = OSTaskCreateExt(&main_button, /* the task function */
          (void *)0,      /* the 'pdata' parameter (not used) */
          &stack_button[(sizeof(stack_button) / sizeof(OS_STK)) - 1], /* ptos */
          OS_LOWEST_PRIO - 3, /* uC/OS-II task priority */
          OS_LOWEST_PRIO - 3, /* unique priority is used as the task ID */
          stack_button,   /* pbos */
          (INT32U)(sizeof(stack_button)/sizeof(OS_STK)),/* stack depth */
          (void *)0,      /* pext */
          (INT16U)0);     /* task options */
    Q_ASSERT(err == 0);

    BSP_start(); /* configure and start the interrupts */

    OSStart(); /* start the uC/OS-II scheduler... */
    return 0; /* NOTE: the scheduler does NOT return */
}

/*******************************************************************************
* NOTE1:
* The call to uC/OS-II API OSTaskCreateExt() assumes that the pointer to the
* top-of-stack (ptos) is at the end of the provided stack memory. This is
* correct only for CPUs with downward-growing stack, but must be changed for
* CPUs with upward-growing stack.
*/

/*
The lesson continues with event-driven programming, focusing on real-time embedded systems.
Previously, event-driven concepts were explained in the context of Windows GUI.
Now, the goal is to transition from traditional RTOS-based programming to event-driven active objects.

The starting point is a MicroC/OS-II-based system with a sequential "superloop" structure.
The system runs two threads: one for blinking an LED and another for handling a button press.
The threads synchronize using semaphores and share a variable protected by a mutex.
This traditional RTOS approach leads to issues like blocking and race conditions.

A better alternative is using the "Active Object" pattern, which eliminates shared-state concurrency.
Active objects encapsulate private data and communicate asynchronously through event queues.
This eliminates the need for mutual exclusion mechanisms, improving efficiency and predictability.

An "Active Object" framework called MicroC/AO is introduced.
It builds on MicroC/OS-II but provides an event-driven structure.
Each active object has a private thread and an event queue.
Instead of waiting for events, objects receive and process them asynchronously.

The button-handling thread is converted into an active object.
The sequential blocking approach is replaced with event-driven processing.
Events like BUTTON_PRESSED and BUTTON_RELEASED trigger the corresponding actions.
The new design eliminates the need for semaphores and improves responsiveness.

Next, the LED-blinking thread is converted into an active object.
Time delays are replaced with "Time Events," which trigger after a set duration.
A new TimeEvent class is introduced to handle periodic or one-shot timeouts.
These events are posted to the active object instead of using blocking delays.

The final improvement combines the button and blinky objects into a single active object.
With event-driven processing, there is no need for separate threads.
This eliminates shared variables and mutexes, simplifying the design and improving efficiency.

The new approach shows that active objects hold more functionality than traditional threads.
They allow for better resource management and eliminate unnecessary blocking.
This shift from traditional RTOS programming to event-driven active objects is a major paradigm shift.

The lesson demonstrates how an event-driven framework differs from an RTOS.
Most RTOS mechanisms are not suitable for active objects due to their reliance on blocking.
While an RTOS can be used to implement an event-driven system, it is not the best fit.

This concludes the lesson, showing how event-driven programming can transform real-time embedded systems.
*/