/* https://www.keil.com/pack/doc/CMSIS_Dev/Core/html/group__NVIC__gr.html for more information about the interrupts
This lesson is about interupts.
polling/busy wait is when a loop continue all the time untill somthing break it. like with the delayfunc() i made.
inturpts with timers is a better method to reduce waste of cpu cycles.
SysTick timer (STK)
The processor has a 24-bit system timer (2^24H=16MHz), SysTick, that counts down from the reload value to
zero, reloads (wraps to) the value in the STK_LOAD register on the next clock edge, then
counts down on subsequent clocks, using the CPU clock.
SysTick->LOAD = the value to count to.
SysTick->VAL = initial value of the timer.
SysTick->CTRL = configuration enable counter, counting down to zero, using AHB processor clock.

in cortexm micro processors the interrupt handler are just like any C function unlike other boards!
they use speacial value to go back to the main code after interrupts unlike other boards that use iret (interrupt return)
*/


#include "stm32f4xx.h"
#include <stdint.h>
#include "header.h"

int main(){
	SysTick->LOAD=SYS_CLOCK_HZ/2-1U; //divider 8M/16Mhz = 0.5 Second (count to to 8M in a 16Mhz timer)
	SysTick->VAL=0U;
	SysTick->CTRL=(1U<<2)|(1U<<1)|(1U<<0);
	__enable_irq();
	ledSetUp();
	*((unsigned volatile long *)(myGPIOD_base+0x18)) = LED_GREEN;
	while(1){
		__asm("NOP");
	}
	return 0;
}
/* The lesson explores how ARM Cortex-M addresses two key requirements for interrupts: saving the correct CPU registers and returning from interrupt functions. 
Unlike the MSP430, ARM Cortex-M allows interrupt handlers to be regular C functions due to the way it handles the stack and register saving. 
This is achieved through a special value in the LR register (0xFFFFFFF9), which signals an interrupt return.

Samek walks through the process of manually triggering the SysTick interrupt and analyzing the stack frame to see how registers are saved and restored. 
He also highlights how the ARM Cortex-M uses a data-driven approach, rather than a special instruction, to return from interrupts, making it more flexible.
Additionally, the lesson covers the impact of the Floating Point Unit (FPU) on interrupt handling, showing that enabling the FPU increases the stack frame size and requires a different return value in the LR register (0xFFFFFFE9). 
Samek concludes by emphasizing the need to properly size the stack when using the FPU and hints at upcoming lessons on race conditions.
Special Data for Interrupt Return:
ARM Cortex-M handles interrupt returns using a unique approach based on data rather than a special instruction. Specifically, it uses a special value loaded into the Link Register (LR) to signal the processor that it should return from an interrupt. For example, when the LR register contains the value 0xFFFFFFF9, the Cortex-M hardware interprets this as a return from interrupt, and the processor restores the previous state, including the Program Counter (PC) and other registers. This method is different from other processors like the MSP430, which use a specific interrupt return instruction like iret.

Register Banking:
ARM Cortex-M features "register banking" for the stack pointer, meaning the CPU can switch between two different stack pointers depending on its internal state. 
These are:

MSP (Main Stack Pointer): Used in Handler mode, which is the state when the CPU is handling an exception (like an interrupt).
PSP (Process Stack Pointer): Used in Thread mode, when the CPU is executing regular code (like your main function).
This distinction allows ARM Cortex-M to efficiently manage different execution contexts, which is particularly important in systems using a Real-Time Operating System (RTOS).

Real-Time Operating System (RTOS) and ARM-Specials:
The concept of register banking, especially the use of separate stack pointers (MSP and PSP), becomes significant in the context of an RTOS. 
In an RTOS, tasks may be switched rapidly and frequently, and each task can have its own stack, managed by the RTOS. 
ARM Cortex-M's ability to switch stack pointers based on the current execution mode (Handler vs. Thread) helps facilitate context switching, which is crucial for the operation of an RTOS.
*/
