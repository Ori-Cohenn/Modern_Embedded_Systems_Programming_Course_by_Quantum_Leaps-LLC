/* This lesson is about the Optimal Implementation in C - State Handlers.

DWT = Debug Watchpoint and Trace register at adress 0xE0001000, at 0xE0001004 is the clock cycle counter.
 Fsm_dispatch(&me->super, e) function took 0x120 clock cycles that is 228 in decimal.
 at last lesson - I used state - table that took 208 clock cycles but was less compitable for adding lot of statets.
state-handler method is almost as fast, while being much more readable, 
maintainable, and significantly smaller in terms of memory, 
because you don't need the big and sparse state-table.
*/


/* TimeBomb/Button with uC/OS-II RTOS */
#include "uc_ao.h"  /* uC/AO API */
#include "bsp.h"
#include <stdbool.h>

Q_DEFINE_THIS_MODULE("main") /* this module name for Q_ASSERT() */

/* The TimeBomb AO =======================================================*/
typedef struct TimeBomb TimeBomb;
struct TimeBomb {
    Active super; /* inherit Active base class */
    /* add private data for the AO... */
    TimeEvent te;
    uint32_t blink_ctr;
};

State TimeBomb_initial(TimeBomb * const me, Event const * const e);
State TimeBomb_wait4button(TimeBomb * const me, Event const * const e);
State TimeBomb_blink(TimeBomb * const me, Event const * const e);
State TimeBomb_pause(TimeBomb * const me, Event const * const e);
State TimeBomb_boom(TimeBomb * const me, Event const * const e);


State TimeBomb_initial(TimeBomb * const me, Event const * const e) {
    return TRAN(TimeBomb_wait4button);
}


State TimeBomb_wait4button(TimeBomb * const me, Event const * const e) {
    State status;
    switch (e->sig) {
        case ENTRY_SIG: {
            BSP_ledGreenOn();
            status = HANDLED_STATUS;
            break;
        }
        case EXIT_SIG: {
            BSP_ledGreenOff();
            status = HANDLED_STATUS;
            break;
        }
        case BUTTON_PRESSED_SIG: {
            me->blink_ctr = 5U;
            status = TRAN(TimeBomb_blink);
            break;
        }
        default: {
            status = IGNORED_STATUS;
            break;
        }
    }
    return status;
}

State TimeBomb_blink(TimeBomb * const me, Event const * const e) {
    State status;
    switch (e->sig) {
        case ENTRY_SIG: {
            BSP_ledRedOn();
            TimeEvent_arm(&me->te, OS_TICKS_PER_SEC/2, 0U);
            status = HANDLED_STATUS;
            break;
        }
        case EXIT_SIG: {
            BSP_ledRedOff();
            status = HANDLED_STATUS;
            break;
        }
        case TIMEOUT_SIG: {
            status = TRAN(TimeBomb_pause);
            break;
        }
        default: {
            status = IGNORED_STATUS;
            break;
        }
    }
    return status;
}

State TimeBomb_pause(TimeBomb * const me, Event const * const e) {
    State status;
    switch (e->sig) {
        case ENTRY_SIG: {
            TimeEvent_arm(&me->te, OS_TICKS_PER_SEC/2, 0U);
            status = HANDLED_STATUS;
            break;
        }
        case TIMEOUT_SIG: {
            --me->blink_ctr;
            if (me->blink_ctr > 0U) {
                status = TRAN(TimeBomb_blink);
            }
            else {
                status = TRAN(TimeBomb_boom);
            }
            break;
        }
        default: {
            status = IGNORED_STATUS;
            break;
        }
    }
    return status;
}

State TimeBomb_boom(TimeBomb * const me, Event const * const e) {
    State status;
    switch (e->sig) {
        case ENTRY_SIG: {
            BSP_ledRedOn();
            BSP_ledGreenOn();
            BSP_ledBlueOn();
            status = HANDLED_STATUS;
            break;
        }
        default: {
            status = IGNORED_STATUS;
            break;
        }
    }
    return status;
}


void TimeBomb_ctor(TimeBomb * const me) {
    Active_ctor(&me->super, (StateHandler)&TimeBomb_initial);
    TimeEvent_ctor(&me->te, TIMEOUT_SIG, &me->super);
}

OS_STK stack_timeBomb[100]; /* task stack */
static Event *timeBomb_queue[10];
static TimeBomb timeBomb;
Active *AO_TimeBomb = &timeBomb.super;

/* the main function =========================================================*/
int main() {
    INT8U err;

    BSP_init(); /* initialize the BSP */
    OSInit();   /* initialize uC/OS-II */

    /* create AO and start it */
    TimeBomb_ctor(&timeBomb);
    Active_start(AO_TimeBomb,
                 2U,
                 timeBomb_queue,
                 sizeof(timeBomb_queue)/sizeof(timeBomb_queue[0]),
                 stack_timeBomb,
                 sizeof(stack_timeBomb),
                 0U);

    BSP_start(); /* configure and start the interrupts */

    OSStart(); /* start the uC/OS-II scheduler... */
    return 0; /* NOTE: the scheduler does NOT return */
}


/*
This lesson presents Miro Samek's approach to optimal state machine implementation in C for embedded systems. The core focus is on the state-handler implementation method,
 which is positioned as superior to previously covered techniques (nested-switch statement and state table).
Development Process

Domain Specific Language (DSL) Creation: The instructor begins by designing a text-based representation of state machines that directly mirrors state diagrams.
C Implementation: This DSL is transformed into valid C code with minimal syntax additions.
State-Handler Pattern: Each state becomes a function (state-handler) that processes events and returns status information.
Framework Integration: The implementation is integrated into the Micro-C-AO active object framework through refactoring.

Key Technical Elements

State Representation: States are implemented as functions rather than enumeration values
State Variable: Uses pointer-to-function instead of enumeration
Transition Mechanism: TRAN() macro assigns a new state-handler to the state variable
Class Hierarchy: Creates FSM (Finite State Machine) base class, inherited by Active class
Event Processing: Dedicated dispatch() operation handles events and state transitions

Advantages Over Other Methods

Readability: Code structure directly maps to state diagram elements
Maintainability: Easier to add/modify states and transitions
Memory Efficiency: Eliminates sparse state tables
Performance: Only 10% slower than state-table method (226 vs. 206 cycles for test case)
Abstraction: Works at the higher conceptual level of state machine elements
Reusability: Implementation is generic enough to be part of the framework

Implementation Details

Uses pointer-to-function for state representation
Handles entry/exit actions via special event signals
Implements state transitions through explicit function calls
Creates a proper inheritance hierarchy with FSM as base class
Uses C-language comma operator for elegant TRAN() macro implementation

Comparison to SMC
The lesson concludes by comparing this approach to the State Machine Compiler (SMC), 
noting that the state-handler method is simpler and more direct while maintaining good performance characteristics. 
SMC generates more complex code based on the object-oriented State Pattern,
 categorized as a "Data-Based strategy" versus the simpler "Code-Based strategy" of state-handlers.
The instructor considers the state-handler method optimal for embedded systems, offering the best balance of readability, maintainability,
 and performance, which is why it's now incorporated directly into the Micro-C-AO framework.



*/