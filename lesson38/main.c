/*
This is the fourth lesson about state machines, focusing on state tables. simmilar to Moore state machine.
 */

/* TimeBomb/Button with uC/OS-II RTOS */
#include "uc_ao.h"  /* uC/AO API */
#include "bsp.h"
#include <stdbool.h>

Q_DEFINE_THIS_MODULE("main") /* this module name for Q_ASSERT() */

/* The TimeBomb AO =======================================================*/
typedef struct {
    Active super; /* inherit Active base class */
    /* add private data for the AO... */
    enum {
        WAIT4BUTTON_STATE,
        BLINK_STATE,
        PAUSE_STATE,
        BOOM_STATE,
        /* ... */
        MAX_STATE
    } state; /* the "state variable" */

    TimeEvent te;
    uint32_t blink_ctr;
} TimeBomb;

typedef enum { TRAN_STATUS, HANDLED_STATUS, IGNORED_STATUS, INIT_STATUS } Status;
/*
TRAN_STATUS will tell you that a transition was taken;

HANDLED_STATUS will tell you that an action was handled but without taking a transition

IGNORED_STATUS will tell you that an event was ignored;

INIT_STATUS will tell you that the initial transition was taken

*/

static Status TimeBomb_init(TimeBomb * const me, Event const * const e) {
    me->state = WAIT4BUTTON_STATE;
    return INIT_STATUS;
}

static Status TimeBomb_wait4button_ENTRY(TimeBomb * const me, Event const * const e) {
    BSP_ledGreenOn();
    return HANDLED_STATUS;
}

static Status TimeBomb_wait4button_EXIT(TimeBomb * const me, Event const * const e) {
    BSP_ledGreenOff();
    return HANDLED_STATUS;
}

static Status TimeBomb_wait4button_PRESSED(TimeBomb * const me, Event const * const e) {
    me->blink_ctr = 5U;
    me->state = BLINK_STATE;
    return TRAN_STATUS;
}

static Status TimeBomb_blink_ENTRY(TimeBomb * const me, Event const * const e) {
    BSP_ledRedOn();
    TimeEvent_arm(&me->te, OS_TICKS_PER_SEC/2, 0U);
    return HANDLED_STATUS;
}

static Status TimeBomb_blink_EXIT(TimeBomb * const me, Event const * const e) {
    BSP_ledRedOff();
    return HANDLED_STATUS;
}

static Status TimeBomb_blink_TIMEOUT(TimeBomb * const me, Event const * const e) {
    me->state = PAUSE_STATE;
    return TRAN_STATUS;
}

static Status TimeBomb_pause_ENTRY(TimeBomb * const me, Event const * const e) {
    TimeEvent_arm(&me->te, OS_TICKS_PER_SEC/2, 0U);
    return HANDLED_STATUS;
}

static Status TimeBomb_pause_TIMEOUT(TimeBomb * const me, Event const * const e) {
    --me->blink_ctr;
    if (me->blink_ctr > 0U) {
        me->state = BLINK_STATE;
        return TRAN_STATUS;
    }
    else {
        me->state = BOOM_STATE;
        return TRAN_STATUS;
    }
}

static Status TimeBomb_boom_ENTRY(TimeBomb * const me, Event const * const e) {
    BSP_ledRedOn();
    BSP_ledGreenOn();
    BSP_ledBlueOn();
	BSP_ledOrangeOn();
    return HANDLED_STATUS;
}

static Status TimeBomb_ignore(TimeBomb * const me, Event const * const e) {
    return IGNORED_STATUS;
}


typedef Status (*TimeBombAction)(TimeBomb * const me, Event const * const e);

static TimeBombAction const TimeBomb_table[MAX_STATE][MAX_SIG] = {
               /*   INIT            | ENTRY                      | EXIT                      | PRESSED                      | RELEASED        | TIMEOUT */
/* wait4button */ { &TimeBomb_init,   &TimeBomb_wait4button_ENTRY, &TimeBomb_wait4button_EXIT, &TimeBomb_wait4button_PRESSED, &TimeBomb_ignore, &TimeBomb_ignore },
/* blink       */ { &TimeBomb_ignore, &TimeBomb_blink_ENTRY,       &TimeBomb_blink_EXIT,       &TimeBomb_ignore,              &TimeBomb_ignore, &TimeBomb_blink_TIMEOUT },
/* pause       */ { &TimeBomb_ignore, &TimeBomb_pause_ENTRY,       &TimeBomb_ignore,           &TimeBomb_ignore,              &TimeBomb_ignore, &TimeBomb_pause_TIMEOUT },
/* boom        */ { &TimeBomb_ignore, &TimeBomb_boom_ENTRY,        &TimeBomb_ignore,           &TimeBomb_ignore,              &TimeBomb_ignore, &TimeBomb_ignore }
};


static void TimeBomb_dispatch(TimeBomb * const me, Event const * const e) {
    Status stat;
    int prev_state = me->state; /* save for later */

    Q_ASSERT((me->state < MAX_STATE) && (e->sig < MAX_SIG));
    stat = (*TimeBomb_table[me->state][e->sig])(me, e);

    if (stat == TRAN_STATUS) { /* transition taken? */
        Q_ASSERT(me->state < MAX_STATE);
        (*TimeBomb_table[prev_state][EXIT_SIG])(me, (Event *)0);
        (*TimeBomb_table[me->state][ENTRY_SIG])(me, (Event *)0);
    }
    else if (stat == INIT_STATUS) { /* initial transition? */
        (*TimeBomb_table[me->state][ENTRY_SIG])(me, (Event *)0);
    }
}

void TimeBomb_ctor(TimeBomb * const me) {
    Active_ctor(&me->super, (DispatchHandler)&TimeBomb_dispatch);
    TimeEvent_ctor(&me->te, TIMEOUT_SIG, &me->super);
    me->state = WAIT4BUTTON_STATE;
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

/*******************************************************************************
* NOTE1:
* The call to uC/OS-II API OSTaskCreateExt() assumes that the pointer to the
* top-of-stack (ptos) is at the end of the provided stack memory. This is
* correct only for CPUs with downward-growing stack, but must be changed for
* CPUs with upward-growing stack.
*/

/*
This is the fourth lesson about state machines, focusing on state tables.
The lesson builds upon previous knowledge of nested-switch statement implementations.
Earlier lessons covered straightforward implementations using nested-switch statements for state machines.
The time-bomb project from lesson 36 used this basic implementation method.
State machines were previously coded in a single dispatch function with switch statements.
The switch statement checked the state-variable, and each case handled different states.
Each state contained another nested switch for handling event signals.
The Blinky-Button project used a simpler variation without the second nested switch.
This lesson introduces state tables as an alternative implementation method.
State tables are popular representations of state machines in the industry.
The lesson demonstrates how to convert truth tables into state tables.
State tables can be organized in both one-dimensional and two-dimensional formats.
The two-dimensional format lists states vertically and events horizontally.
Each cell contains the next-state and action information for specific state-event combinations.
The lesson applies this technique to convert the Time-Bomb software into a state table format.
The implementation uses C's native support for two-dimensional arrays.
The code uses special MAX constants to track array dimensions automatically.
Each table entry contains a function pointer called TimeBombAction.
The state table is designed to be constant and stored in ROM rather than RAM.
The lesson also introduces entry and exit actions for states.
Entry and exit actions help eliminate repetitive code in state transitions.
The improved design converts the TimeBomb from a Mealy-type to mostly Moore-type state machine.
Moore-type machines with state-associated actions are generally more maintainable.
The implementation adds special ENTRY and EXIT signals to handle state transitions.
The system uses status returns to track transition states and handle entry/exit actions.
The state table approach offers good runtime performance and regular structure.
However, the implementation has drawbacks like code fragmentation and sparse tables.
Adding new states or events requires significant table modifications.
The next lesson will cover a more optimal implementation using a reusable event processor.
*/