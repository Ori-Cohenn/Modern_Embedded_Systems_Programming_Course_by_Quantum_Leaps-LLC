/****************************************************************************
 * MInimal Real-time Operating System (MIROS)
 * version 0.25
 * by Ori Cohen learned in quantum leaps, LLC's course.
 * This software is a learning aid to illustrate the concepts underlying
 * a Real-Time Operating System (RTOS). The main goal of the software is
 * simplicity and clear presentation of the concepts, but without dealing
 * with various corner cases, portability, or error handling. For these
 * reasons, the software is generally NOT intended or recommended for use
 * in commercial applications.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 *
 ****************************************************************************/
#ifndef MIROS_H
#define MIROS_H

/* Thread Control Block (TCB) */
typedef struct
{
    void *sp; /* stack pointer */
	uint32_t timeout; /* timeout delay down-counter */
    /* ... other attributes associated with a thread */
} OSThread;

typedef void (*OSThreadHandler)();

void OS_init(void *stkSto, uint32_t stkSize);

/* callback to handle the idle condition */
void OS_onIdle(void);

/* this function must be called with interrupts DISABLED */
void OS_sched(void);

/* transfer control to the RTOS to run the threads */
void OS_run(void);

/* blocking delay */
void OS_delay(uint32_t ticks);

/* process all timeouts */
void OS_tick(void);

/* callback to configure and start interrupts */
void OS_onStartup(void);

/* me is a pointer to the tcb , OSThreadHandler is a pointer to */
void OSThread_start(
    OSThread *me,
    OSThreadHandler threadHandler,
    void *stkSto, uint32_t stkSize);

#endif /* MIROS_H */
