/*
	this lesson is about toggle the led with arrays and pointers.
	0x1UL, UL is for unigned long.
	#define GPIO_MODER_MODER12_Pos           (24U) gpio 12 on the moder                                 
	#define GPIO_MODER_MODER12_Msk           (0x3UL << GPIO_MODER_MODER12_Pos)     set as output
	#define GPIO_MODER_MODER12               GPIO_MODER_MODER12_Msk  
	Stack memory in an MCU is used for managing function calls, local variables, 
	and interrupt handling. It stores return addresses,
	function parameters, and automatic variables, ensuring efficient memory usage. 
	The stack pointer (SP-R13) register tracks the top of the stack,
	maintaining stack integrity during push and pop operations.
	This setup enables proper function execution and context switching.
*/

#include "stm32f4xx.h"

#define myGPIOD_base	 0x40020C00				
#define LED_GREEN	 		(1UL<<12)
#define LED_GREEN_OFF	(1U<<(12+16))
#define LED_ORANGE 		(1U<<13)
#define LED_RED		 		(1U<<14)
#define LED_RED_OFF		(1U<<(14+16))
#define LED_BLUE	 		(1U<<15)

int delayfunc(int delay);
int turnOffAllLeds();
int ledSetUp();

int delayfunc(int delay){
	for(volatile int i=0;i<delay;i++) __asm("NOP");
	return 0;
}
/*
void turnOffAllLeds(){
	GPIOD->BSRR =  1<<(12+16); // Set the BSRR bit 12 + 16->F to 1 to turn respective LED off
	GPIOD->BSRR =  1<<(13+16); // Set the BSRR bit 13 + 16 to 1 to turn respective LED off
	GPIOD->BSRR =  1<<(14+16); // Set the BSRR bit 14 + 16 to 1 to turn respective LED off
	GPIOD->BSRR =  1<<(15+16); // Set the BSRR bit 15 + 16 to 1 to turn respective LED off
}
*/
int turnOffAllLeds(){
	GPIOD->BSRR &=~LED_GREEN;
	GPIOD->BSRR &=~LED_ORANGE;
	GPIOD->BSRR &=~LED_RED;
	GPIOD->BSRR &=~LED_BLUE;
	return 0;
}
int ledSetUp(){
	// Configue LEDs
	//RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // Enable the clock of port D of the GPIO-> 0x1UL << RCC_AHB1ENR_GPIODEN_Pos=3u->0x0...08
	RCC->AHB1ENR |=	(1UL<<3);
	GPIOD->MODER |= GPIO_MODER_MODER12_0; // Green LED, set pin 12 as output
	GPIOD->MODER |= GPIO_MODER_MODER13_0; // Orange LED, set pin 13 as output
	GPIOD->MODER |= GPIO_MODER_MODER14_0; // Red LED, set pin 14 as output
	GPIOD->MODER |= GPIO_MODER_MODER15_0; // Blue LED, set pin 15 as output
	// Turn off LEDs
	turnOffAllLeds();
	return 0;
}

int main(){
	ledSetUp();
	//read-modify-write cycle ORR(32 bit OR op),BIC (32 bit NAND op)
	/*
	*((unsigned volatile long *)(myGPIOD_base+0x14)) |= LED_GREEN;
	while(1){
		*((unsigned volatile long *)(myGPIOD_base+0x14)) |= LED_RED; // 0x14 is odr reg for output data!
		delayfunc(1<<21);
		*((unsigned volatile long *)(myGPIOD_base+0x14)) &=~ LED_RED;
		delayfunc(1<<21);
	}
	*/
		// a single and atomeic write op-> BSSR
	*((unsigned volatile long *)(myGPIOD_base+0x18)) = LED_GREEN;
		while(1){
		*((unsigned volatile long *)(myGPIOD_base+0x18)) = LED_RED; // 0x18 is BSSR reg for  bit set/reset register!
		delayfunc(1<<21);
		*((unsigned volatile long *)(myGPIOD_base+0x18)) = LED_RED_OFF;
		delayfunc(1<<21);
	}
	return 0;
}

