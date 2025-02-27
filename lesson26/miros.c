/****************************************************************************
 * MInimal Real-time Operating System (MIROS)
 * version 0.26
 * by Ori Cohen learned in quantum leaps, LLC's course.
 * This software is a teaching aid to illustrate the concepts underlying
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
#include <stdint.h>
#include "miros.h"
#include "qassert.h"

Q_DEFINE_THIS_FILE


OSThread *volatile OS_curr; /* pointer to the current thread */
OSThread *volatile OS_next; /* pointer to the next thread to run */

OSThread *OS_thread[32+1]; /* array of threads started so far */
uint32_t OS_readySet; /*bitmask of threads that are ready to run */
uint32_t OS_delayedSet; /* bitmask of threads that are delayed */

//circler behavior of threads using arrays instead of linked list.

#define LOG2(x) (32U-__builtin_clz(x))

OSThread idleThread;
void main_idleThread()
{
  while (1)
  {
	OS_onIdle();
  }
}

void OS_init(void *stkSto, uint32_t stkSize)
{
	/* set the PendSV interrupt priority to the lowest level 0xFF */
	*(uint32_t volatile *)0xE000ED20 |= (0xFFU << 16);
	/* start idleThread thread */
	OSThread_start(&idleThread,
					0U, /* idle thread priority */
					&main_idleThread,
					stkSto, stkSize);
}

void OS_sched(void) //round-robin scheduling
{
    /* choose the next thread to execute... */
    OSThread *next;
	/* OS_next = ... sched=schedule the interupts.  */
	/* first example of hard code automatic thread scheduling
	extern OSThread blinky1,blinky2;
	if(OS_curr==&blinky1){
		OS_next=&blinky2;
	}
	else{
		OS_next=&blinky1;
	}
	*/
    if(OS_readySet==0U){ /*idle condition?*/
		next = OS_thread[0]; /* index of the idle thread */
	}
	else{
		next=OS_thread[LOG2(OS_readySet)];
		Q_ASSERT(next != (OSThread *)0);
	}

    /* trigger PendSV, if needed */
    if (next != OS_curr) {
        OS_next = next;
        *(uint32_t volatile *)0xE000ED04 = (1U << 28);//PendSV exception
    }
}


void OS_run(void) {
    /* callback to configure and start interrupts */
    OS_onStartup();

    __asm volatile ("cpsid i"); //disable inturreputs
    OS_sched();
    __asm volatile ("cpsie i"); //enable inturreputs

    /* the following code should never execute */
    Q_ERROR();
}

void OS_tick(void){
	uint32_t workingSet = OS_delayedSet;
	while (workingSet != 0U) {
		OSThread *t = OS_thread[LOG2(workingSet)];
		uint32_t bit;
		Q_ASSERT((t != (OSThread *)0) && (t->timeout != 0U));

		bit = (1U << (t->prio - 1U));
		--t->timeout;
		if (t->timeout == 0U) {
			OS_readySet   |= bit;  /* insert to set */
			OS_delayedSet &= ~bit; /* remove from set */
		}
		workingSet &= ~bit; /* remove from working set */
	}
}

void OS_delay(uint32_t ticks){
	uint32_t bit;
    __asm volatile ("cpsid i"); //disable inturreputs
	
	/*never call OS delay from the idleThread */
	Q_REQUIRE(OS_curr != OS_thread[0]);
	
    OS_curr->timeout = ticks;
    bit = (1U << (OS_curr->prio - 1U));
    OS_readySet &= ~bit;
    OS_delayedSet |= bit;
	OS_sched();
    __asm volatile ("cpsie i"); //enable inturreputs
}

void OSThread_start(
	OSThread *me,
	uint8_t prio, /* thread priority */
	OSThreadHandler threadHandler,
	void *stkSto, uint32_t stkSize)
{
	/* round down the stack top to the 8-byte boundary by muliply and devide
	 * NOTE: ARM Cortex-M stack grows down from hi -> low memory
	 */
	uint32_t *sp = (uint32_t *)((((uint32_t)stkSto + stkSize) / 8) * 8);
	uint32_t *stk_limit;

	/* priority must be in ragne
    * and the priority level must be unused
    */
    Q_REQUIRE((prio < Q_DIM(OS_thread))
              && (OS_thread[prio] == (OSThread *)0));
	
	
	*(--sp) = (1U << 24);			   /* xPSR */
	*(--sp) = (uint32_t)threadHandler; /* PC */
	*(--sp) = 0x0000000EU;			   /* LR  */
	*(--sp) = 0x0000000CU;			   /* R12 */
	*(--sp) = 0x00000003U;			   /* R3  */
	*(--sp) = 0x00000002U;			   /* R2  */
	*(--sp) = 0x00000001U;			   /* R1  */
	*(--sp) = 0x00000000U;			   /* R0  */
	/* additionally, fake registers R4-R11 */
	*(--sp) = 0x0000000BU; /* R11 */
	*(--sp) = 0x0000000AU; /* R10 */
	*(--sp) = 0x00000009U; /* R9 */
	*(--sp) = 0x00000008U; /* R8 */
	*(--sp) = 0x00000007U; /* R7 */
	*(--sp) = 0x00000006U; /* R6 */
	*(--sp) = 0x00000005U; /* R5 */
	*(--sp) = 0x00000004U; /* R4 */

	/* save the top of the stack in the thread's attibute */
	me->sp = sp;

	/* round up the bottom of the stack to the 8-byte boundary */
	stk_limit = (uint32_t *)(((((uint32_t)stkSto - 1U) / 8) + 1U) * 8);

	/* pre-fill the unused part of the stack with 0xDEADBEEF */
	for (sp = sp - 1U; sp >= stk_limit; --sp)
	{
		*sp = 0xDEADBEEFU;
	}

	/* register the thread with the OS */
    OS_thread[prio] = me;
	me->prio=prio;
	/* make the thread ready to run */
    if (prio > 0U) {
        OS_readySet |= (1U << (prio - 1U));
    }
}

/* inline assembly syntax for Compiler 6 (ARMCLANG) */
__attribute__((naked)) void PendSV_Handler(void)
{
	__asm volatile(
		/* __disable_irq(); */
		"  CPSID         I                 \n"

		/* if (OS_curr != (OSThread *)0) { */
		"  LDR           r1,=OS_curr       \n"
		"  LDR           r1,[r1,#0x00]     \n"
		"  CMP           r1,#0             \n"
		"  BEQ           PendSV_restore    \n"

	/*     push registers r4-r11 on the stack */
#if (__ARM_ARCH == 6)						  // if ARMv6-M...
		"  SUB           sp,sp,#(8*4)     \n" // make room for 8 registers r4-r11
		"  MOV           r0,sp            \n" // r0 := temporary stack pointer
		"  STMIA         r0!,{r4-r7}      \n" // save the low registers
		"  MOV           r4,r8            \n" // move the high registers to low registers...
		"  MOV           r5,r9            \n"
		"  MOV           r6,r10           \n"
		"  MOV           r7,r11           \n"
		"  STMIA         r0!,{r4-r7}      \n" // save the high registers
#else										  // ARMv7-M or higher
		"  PUSH          {r4-r11}          \n"
#endif										  // ARMv7-M or higher

		/*     OS_curr->sp = sp; */
		"  LDR           r1,=OS_curr       \n"
		"  LDR           r1,[r1,#0x00]     \n"
		"  MOV           r0,sp             \n"
		"  STR           r0,[r1,#0x00]     \n"
		/* } */

		"PendSV_restore:                   \n"
		/* sp = OS_next->sp; */
		"  LDR           r1,=OS_next       \n"
		"  LDR           r1,[r1,#0x00]     \n"
		"  LDR           r0,[r1,#0x00]     \n"
		"  MOV           sp,r0             \n"

		/* OS_curr = OS_next; */
		"  LDR           r1,=OS_next       \n"
		"  LDR           r1,[r1,#0x00]     \n"
		"  LDR           r2,=OS_curr       \n"
		"  STR           r1,[r2,#0x00]     \n"

	/* pop registers r4-r11 */
#if (__ARM_ARCH == 6)						   // if ARMv6-M...
		"  MOV           r0,sp             \n" // r0 := top of stack
		"  MOV           r2,r0             \n"
		"  ADDS          r2,r2,#(4*4)      \n" // point r2 to the 4 high registers r7-r11
		"  LDMIA         r2!,{r4-r7}       \n" // pop the 4 high registers into low registers
		"  MOV           r8,r4             \n" // move low registers into high registers
		"  MOV           r9,r5             \n"
		"  MOV           r10,r6            \n"
		"  MOV           r11,r7            \n"
		"  LDMIA         r0!,{r4-r7}       \n" // pop the low registers
		"  ADD           sp,sp,#(8*4)      \n" // remove 8 registers from the stack
#else										   // ARMv7-M or higher
		"  POP           {r4-r11}          \n"
#endif										   // ARMv7-M or higher

		/* __enable_irq(); */
		"  CPSIE         I                 \n"

		/* return to the next thread */
		"  BX            lr                \n");
}