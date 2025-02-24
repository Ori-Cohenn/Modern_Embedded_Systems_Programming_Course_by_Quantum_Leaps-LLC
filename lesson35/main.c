/*
This lesson is the first part of a series of subject on State Machines.
 */

/* BlinkyButton/Button with uC/AO active-object framework */
#include "uc_ao.h"  /* uC/AO API */
#include "bsp.h"
#include <stdbool.h>

Q_DEFINE_THIS_MODULE("main") /* this module name for Q_ASSERT() */


/* The BlinkyButton =========================================================*/
enum { INITIAL_BLINK_TIME = (OS_TICKS_PER_SEC/4)};

typedef struct {
    Active super; /* inherit Active base class */
	 /* add private data for the AO... */
    enum{
			OFF_STATE,
			ON_STATE
	} state; //represntion of states in C using enum (enumeration)
    TimeEvent te;
    uint32_t blink_time;
} BlinkyButton;


static void BlinkyButton_dispatch(BlinkyButton * const me, Event const * const e) {
	if (e->sig == INIT_SIG) {
		 BSP_ledBlueOff();
		 TimeEvent_arm(&me->te, me->blink_time * 3U, 0U);
		 me->state = OFF_STATE;
    }
    switch (me->state) {
        case OFF_STATE: {
            switch (e->sig) {
                case TIMEOUT_SIG: {
                    BSP_ledGreenOn();
                    TimeEvent_arm(&me->te, me->blink_time, 0U);
                    me->state = ON_STATE;
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
            }
            break;
        }
        case ON_STATE: {
            switch (e->sig) {
                case TIMEOUT_SIG: {
                    BSP_ledGreenOff();
                    TimeEvent_arm(&me->te, me->blink_time * 3U, 0U);
                    me->state = OFF_STATE;
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
            }
            break;
        }
        default: {
            Q_ASSERT(0); /* invalid state! */
            break;
        }
    }
}

void BlinkyButton_ctor(BlinkyButton * const me) {
    Active_ctor(&me->super, (DispatchHandler)&BlinkyButton_dispatch);
    TimeEvent_ctor(&me->te, TIMEOUT_SIG, &me->super);
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
State machines provide a structured way to manage system behavior based on events.  

Unlike sequential programming, which relies on blocking calls to maintain context, state machines explicitly track the current state of a system.  

This eliminates the need for improvised flags and variables like `isLedOn`, making the code more organized and predictable.  

A state machine consists of defined *states*, *events*, and *transitions* between states.  

When an event occurs, the state machine processes it according to the defined rules, ensuring structured and efficient behavior.  

The main advantage of state machines is their clear graphical representation, which allows easy visualization of system behavior.  

In UML, states are represented as rounded rectangles, transitions as arrows, and actions are marked using a slash (`/`).  

State machines follow a *Run-to-Completion* (RTC) model, meaning each event is fully processed before handling the next one.  

This matches perfectly with the event-driven approach used in Active Objects, reinforcing the idea that state machines are ideal for specifying their behavior.  

To demonstrate state machines, the BlinkyButton example was implemented using a state machine approach instead of manual flag management.  

Using the QM tool, a state diagram was created, defining two states: "on" and "off," with transitions based on TIMEOUT and BUTTON_PRESSED events.  

Each state transition is explicitly defined with corresponding actions, such as turning the LED on or off and adjusting the blink time.  

The implementation in C involved creating an enumeration for states and updating the Active Object dispatch function to follow the state machine logic.  

A switch-case structure was used to process events based on the current state, making the code directly traceable to the state diagram.  

This implementation improves *extensibility*, making it easier to add new states and events without disrupting existing functionality.  

It also enhances *traceability*, an essential feature for safety-critical software, ensuring clear correspondence between code and design.  

Finally, the new state machine-based implementation was tested on the Tiva LaunchPad board, and it worked identically to the previous version.  

In future lessons, more advanced state machine concepts, including hierarchical state machines, will be explored.  
*/