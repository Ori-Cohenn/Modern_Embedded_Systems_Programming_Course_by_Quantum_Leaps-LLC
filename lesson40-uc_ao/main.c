/*this lesson is about Hierarchical State Machine*/

/* TimeBomb/Button with uC/OS-II RTOS */
#include "uc_ao.h"  /* uC/AO API */
#include "bsp.h"
#include <stdbool.h>

static char const this_module[] = "main"; /* this module name for Q_ASSERT() */

/* The TimeBomb AO =======================================================*/
typedef struct TimeBomb TimeBomb;
struct TimeBomb {
    Active super; /* inherit Active base class */
    /* add private data for the AO... */
    TimeEvent te;
    uint32_t blink_ctr;
};

State TimeBomb_initial(TimeBomb * const me, Event const * const e);
State TimeBomb_armed(TimeBomb * const me, Event const * const e);
State TimeBomb_wait4button(TimeBomb * const me, Event const * const e);
State TimeBomb_blink(TimeBomb * const me, Event const * const e);
State TimeBomb_pause(TimeBomb * const me, Event const * const e);
State TimeBomb_boom(TimeBomb * const me, Event const * const e);
State TimeBomb_defused(TimeBomb * const me, Event const * const e);


State TimeBomb_initial(TimeBomb * const me, Event const * const e) {
    return TRAN(TimeBomb_wait4button);
}

State TimeBomb_armed(TimeBomb * const me, Event const * const e) {
    State status;
    switch (e->sig) {
        case EXIT_SIG: {
            BSP_ledRedOff();
            BSP_ledGreenOff();
			BSP_ledBlueOff();
            status = TRAN(TimeBomb_wait4button);
            break;
        }
        case INIT_SIG: {
            status = TRAN(TimeBomb_wait4button);
            break;
        }
        case BUTTON2_PRESSED_SIG: {
            status = TRAN(TimeBomb_defused);
            break;
        }
        default: {
            status = SUPER(Hsm_top);
            break;
        }
    }
    return status;
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
            status = SUPER(TimeBomb_armed);
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
            status = SUPER(TimeBomb_armed);
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
            status = SUPER(TimeBomb_armed);
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
            status = SUPER(TimeBomb_armed);
            break;
        }
    }
    return status;
}

State TimeBomb_defused(TimeBomb * const me, Event const * const e) {
    State status;
    switch (e->sig) {
        case ENTRY_SIG: {
            BSP_ledBlueOn();
            status = HANDLED_STATUS;
            break;
        }
        case BUTTON2_PRESSED_SIG: {
			BSP_ledBlueOff();
            status = TRAN(TimeBomb_armed);
            break;
        }
        default: {
            status = SUPER(Hsm_top);
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
The Modern Embedded Systems Programming course introduces hierarchical state machines as a solution to the "state-transition explosion" problem.
By nesting states, redundant transitions are eliminated, making state machine logic cleaner and easier to manage.
This lesson builds on the previous "optimal" state machine implementation, allowing easy extension to hierarchical state machines.

A new "defused" state is added to the TimeBomb application, triggered by BUTTON2_PRESSED.
Traditional state machines require duplicating this transition in every state, violating the DRY (Don't Repeat Yourself) principle.
Hierarchical state machines solve this by introducing a higher-level "armed" superstate that handles the common transition.
Any substate within "armed" inherits this transition, avoiding unnecessary duplication.

Coding a hierarchical state machine involves adding a SUPER() macro to specify the superstate.
A new TimeBomb_armed state-handler is introduced, where BUTTON2_PRESSED transitions to "defused".
The event processor is updated to support state hierarchy by modifying Hsm_dispatch() to propagate unhandled events to superstates.

Nested initial transitions are also implemented, ensuring direct transitions to "armed" automatically lead to "wait4button".
Exit actions are added to the "armed" state, ensuring proper cleanup when transitioning out.
These additions improve the state machine design without significantly increasing complexity.

However, the toy Micro-C-AO framework lacks full support for hierarchical state machines.
Rather than implementing these complex features from scratch, the application is ported to the professional-grade QP/C framework.
QP/C provides a complete implementation of hierarchical state machines and active objects, reducing manual coding effort.

Porting to QP/C involves replacing the Micro-C-AO framework with QP/C components.
Files related to Micro-C-AO are removed, and QP/C source files are added to the project.
Compiler include paths are updated to reference the appropriate QP/C directories.

The Board Support Package (BSP) is adapted to use QP/C APIs, replacing custom macros with QP equivalents.
QACTIVE_POST replaces Active_post, and QF_TICK replaces the manual time event processing.
Callbacks such as QF_onStartup() are used instead of custom startup functions.

The TimeBomb state machine code is updated with QP/C naming conventions.
State handlers use QState instead of State, and transitions use Q_TRAN instead of TRAN.
The per-thread stack requirement is eliminated, simplifying memory management.

The updated project builds cleanly, and testing confirms correct behavior.
Pressing SW1 transitions to the "armed" state, where the Red LED blinks.
Pressing SW2 transitions to "defused", turning the Blue LED on.
Pressing SW2 again returns to "armed" and "wait4button", demonstrating hierarchical state behavior.

The successful port to QP/C proves that hierarchical state machines are not difficult to implement manually with the right approach.
However, manual coding remains labor-intensive and can be automated.
The next lesson will introduce automatic state machine code generation to streamline development.
*/