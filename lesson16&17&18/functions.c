#include "stm32f4xx.h"
#include "header.h"

void SysTick_Handler(void){ //irq in cortexm4 header file (CMSIS)
	GPIOD->ODR ^= LED_BLUE;
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
