/*
 This lesson is about Active Objects in Real-Time Part-1: Run-to-Completion and RMS/RMA
 */

#include "qpc.h"
#include "bsp.h"

Q_DEFINE_THIS_FILE

typedef struct {
    QActive super; /* inherit QActive */

    /* private attribures: */
    QTimeEvt te;
} Blinky1;

void Blinky1_ctor(Blinky1 * const me);

static QState Blinky1_initial(Blinky1 * const me, void const * const par);
static QState Blinky1_active(Blinky1 * const me, QEvt const * const e);

/* Blinky1 constructor */
void Blinky1_ctor(Blinky1 * const me) {
    QActive_ctor(&me->super, Q_STATE_CAST(&Blinky1_initial));
    QTimeEvt_ctorX(&me->te, &me->super, TIMEOUT_SIG, 0U);
}

/* Blinky1 initial pseudostate */
static QState Blinky1_initial(Blinky1 * const me, void const * const par) {
    QTimeEvt_armX(&me->te, 2U, 2U); //With system clock tick occurring every millisecond, this will make Blinky1 periodic with the period of 2 milliseconds.
    return Q_TRAN(&Blinky1_active);
}
/* Blinky1 active state */
QState Blinky1_active(Blinky1 * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        case TIMEOUT_SIG: {
            uint32_t volatile i;
            for (i = 1500U; i != 0U; --i) {
                BSP_ledGreenOn();
                BSP_ledGreenOff();
            }
            status_ = Q_HANDLED();
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}


typedef struct {
    QActive super; /* inherit QActive */
} Blinky2;

void Blinky2_ctor(Blinky2 * const me);

static QState Blinky2_initial(Blinky2 * const me, void const * const par);
static QState Blinky2_active(Blinky2 * const me, QEvt const * const e);

/* Blinky2 constructor */
void Blinky2_ctor(Blinky2 * const me) {
    QActive_ctor(&me->super, Q_STATE_CAST(&Blinky2_initial));
}

/* Blinky2 initial pseudostate */
static QState Blinky2_initial(Blinky2 * const me, void const * const par) {
    return Q_TRAN(&Blinky2_active);
}
/* Blinky2 active state */
QState Blinky2_active(Blinky2 * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        case BUTTON_PRESS_SIG: {
            for (uint32_t volatile i = 3*1500U; i != 0U; --i) {
                BSP_ledBlueOn();
                BSP_ledBlueOff();
            }
            status_ = Q_HANDLED();
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}

QEvt const *blinky1_queue[10]; /* queue buffer */
Blinky1 blinky1;

QEvt const *blinky2_queue[10]; /* queue buffer */
Blinky2 blinky2;

QActive * const AO_Blinky1 = &blinky1.super;
QActive * const AO_Blinky2 = &blinky2.super;

int main(void) {
    BSP_init();
    QF_init();

    /* initialize and start blinky1 thread */
    Blinky1_ctor(&blinky1);
    QACTIVE_START(&blinky1,
                   5U, /* priority */
                   blinky1_queue, Q_DIM(blinky1_queue), /* event queue */
                   (void *)0, 0, /* stack memory, stack size (not used) */
                   (void *)0); /* extra parameter (not used) */

    /* initialize and start blinky2 thread */
    Blinky2_ctor(&blinky2);
    QACTIVE_START(&blinky2,
                   2U, /* priority */
                   blinky2_queue, Q_DIM(blinky2_queue), /* event queue */
                   (void *)0, 0, /* stack memory, stack size (not used) */
                   (void *)0); /* extra parameter (not used) */

    /* transfer control to the RTOS to run the threads */
    return QF_run();
}

/*
This lesson focuses on the practical application of event-driven programming using Active Objects in embedded systems development.
The instructor, Miro Samek, builds upon concepts introduced in previous lessons to demonstrate the advantages of Active Objects over traditional threading models.
Starting with a preemptive priority-based RTOS kernel called QXK from the QP/C framework, the lesson demonstrates how traditional threads (blinky-1 and blinky-2) can be converted to Active Objects.
The conversion process involves creating Active Object classes that inherit from the QActive base class, replacing blocking operations with event-driven mechanisms, and implementing simple state machines for each object.
A key insight is that Active Objects don't require individual private stacks since they execute as "basic threads" with run-to-completion semantics, resulting in significant memory savings compared to traditional "extended threads."
Through logic analyzer measurements, the lesson demonstrates that Active Objects can maintain hard real-time deadlines just as effectively as traditional threads.
The higher-priority Blinky1 Active Object successfully preempts the lower-priority Blinky2 Active Object multiple times, proving that run-to-completion semantics does not prevent proper scheduling.
This demonstration confirms that Active Objects are fully compatible with Rate Monotonic Scheduling (RMS) techniques when used with a preemptive priority-based kernel.
In fact, Active Objects may be even more suitable for hard real-time applications than traditional threads because they avoid shared resources by using asynchronous events instead.
The instructor indicates that a future lesson will elaborate on this important aspect of Active Objects in real-time systems.

*/