/*
	this lesson is using multiple files to make an organise code and also
	speeding up the compiltion time because it will re-compile only the files
	that have changed.
	each file compile sepratly.
	when calling a function that didn't difined we get a linker error because it cant found the function
	the compiler in the other hand does accept it can't know in which file it has been difined(as discused above with multiple files).
	LR-> link register, fact() is not a leaf function (main), so LR have to save the caller function.
	also R0 is like a borker for function, its saveing the argument in the function and also returns in the return call.
	all of these rules are  located in AAPCS= ARM Appication Procedure Call Standard.
	R0-R3 and R12 ar used for passing arguments and returning values.
	R4-R11 is presereved and if it used they must be saved on the SP before using them.
	our fact() a deep call sequences used alot of RAM and should be avoided in embedded systems a more efficiante way will be a loop\lookup table.
	
*/

#include "stm32f4xx.h"
#include "delay.h"

#define myGPIOD_base	 0x40020C00				
#define LED_GREEN	 		(1UL<<12)
#define LED_GREEN_OFF	(1U<<(12+16))
#define LED_ORANGE 		(1U<<13)
#define LED_RED		 		(1U<<14)
#define LED_RED_OFF		(1U<<(14+16))
#define LED_BLUE	 		(1U<<15)

int turnOffAllLeds();
int ledSetUp();
unsigned fact(unsigned n);

int turnOffAllLeds(){
	GPIOD->BSRR =  1<<(12+16); // Set the BSRR bit 12 + 16->F to 1 to turn respective LED off
	GPIOD->BSRR =  1<<(13+16); // Set the BSRR bit 13 + 16 to 1 to turn respective LED off
	GPIOD->BSRR =  1<<(14+16); // Set the BSRR bit 14 + 16 to 1 to turn respective LED off
	GPIOD->BSRR =  1<<(15+16); // Set the BSRR bit 15 + 16 to 1 to turn respective LED off
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

unsigned fact(unsigned n){
	// 0!-> 1 , 1!=1, 2!=2*1, n!=n*(n-1)! for n>0
	if(n==0U){
		return 1U;
	}
	else{
	return n*fact(n-1U);
	}
	return 0U;
}

int main(){
	unsigned volatile x;
	x=fact(0U);
	x=2U+3U*fact(1U);
	(void)fact(5U);
	ledSetUp();
	*((unsigned volatile long *)(myGPIOD_base+0x18)) = LED_GREEN;
	while(1){
		*((unsigned volatile long *)(myGPIOD_base+0x18)) = LED_RED; // 0x18 is BSSR reg for  bit set/reset register!
		delayfunc(1<<21);
		*((unsigned volatile long *)(myGPIOD_base+0x18)) = LED_RED_OFF;
		delayfunc(1<<21);
	}
	return 0;
}

