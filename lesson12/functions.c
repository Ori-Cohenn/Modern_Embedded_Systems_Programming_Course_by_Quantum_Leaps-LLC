#include "header.h"
#include "stm32f4xx.h"


int delayfunc(int volatile delay){
	for(volatile int i=0;i<delay;i++) __asm("NOP");
	return 0;
}
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
	unsigned foo[6];
	foo[n]=n;
	if(n==0U){
		return 1U;
	}
	else{
	//return n*fact(n-1U);
	return foo[n]; // index out of bound making the stack to overflow
	}
	return 0U;
}

int *swap(int *x, int *y){
	static int tmp[2]; // no longer part of stack but sitting on the regular RAM memory.
	tmp[0]= *x;
	tmp[1]= *y;
	*x=tmp[1];
	*y=tmp[0];
	return tmp;
}