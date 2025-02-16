/*
This lesson is about Event-Driven Programming, Best practices for concurrency & Active Object pattern
 */

/* BlinkyButton/Button with uC/AO active-object framework */
#include "uc_ao.h"  /* uC/AO API */
#include "bsp.h"
#include <stdbool.h>


/* The BlinkyButton =========================================================*/
enum { INITIAL_BLINK_TIME = (OS_TICKS_PER_SEC/4)};

typedef struct {
    Active super; /* inherit Active base class */
    /* add private data for the AO... */
    TimeEvent te;
    bool isLedOn;
    uint32_t blink_time;
} BlinkyButton;


static void BlinkyButton_dispatch(BlinkyButton * const me, Event const * const e) {
    switch (e->sig) {
        case INIT_SIG:
            BSP_ledBlueOff();
            /* intentionally fall through...*/
        case TIMEOUT_SIG: {
            if (!me->isLedOn) { /* LED not on */
                BSP_ledGreenOn();
                me->isLedOn = true;
                TimeEvent_arm(&me->te, me->blink_time, 0U);
            }
            else {  /* LED is on */
                BSP_ledGreenOff();
                me->isLedOn = false;
                TimeEvent_arm(&me->te, me->blink_time * 3U, 0U);
            }
            break;
        }
        case BUTTON_PRESSED_SIG: {
            INT8U err; /* uC/OS-II error status */

            BSP_ledBlueOn();

            me->blink_time >>= 1; /* shorten the blink time by factor of 2 */
            if (me->blink_time == 0U) {
                me->blink_time = INITIAL_BLINK_TIME;
            }
            break;
        }
        case BUTTON_RELEASED_SIG: {
            BSP_ledBlueOff();
            break;
        }
        default: {
            break;
        }
    }
}
void BlinkyButton_ctor(BlinkyButton * const me) {
    Active_ctor(&me->super, (DispatchHandler)&BlinkyButton_dispatch);
    TimeEvent_ctor(&me->te, TIMEOUT_SIG, &me->super);
    me->isLedOn = false;
    me->blink_time = INITIAL_BLINK_TIME;
}

OS_STK stack_blinkyButton[100]; /* task stack */
static Event *blinkyButton_queue[10]; //memory buffer for the private event queue of the active object
static BlinkyButton blinkyButton; //the instance of the Button active object
Active *AO_BlinkyButton = &blinkyButton.super; /*global pointer to the object so that other parts of the 
code, such as the BSP, can post events to it */


/* the main function =========================================================*/
int main() {
    INT8U err;

    BSP_init(); /* initialize the BSP */
    OSInit();   /* initialize uC/OS-II */

    /* create AO and start it */
    BlinkyButton_ctor(&blinkyButton);
    Active_start(AO_BlinkyButton,
                 2U,
                 blinkyButton_queue,
                 sizeof(blinkyButton_queue)/sizeof(blinkyButton_queue[0]),
                 stack_blinkyButton,
                 sizeof(stack_blinkyButton),
                 0U);

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

A naked thread is a thread that runs without an operating system (OS) or a thread that lacks standard OS-provided management like scheduling or context switching.
It’s called "naked" because it operates without the usual system-level abstractions, meaning the programmer must handle stack management,
scheduling, and synchronization manually.
This is common in embedded systems and real-time applications where minimal overhead and precise control are required.

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