/*
	in this lesson i will turn on the led with bitwize ops and hal library
	#define GPIOD               ((GPIO_TypeDef *) GPIOD_BASE)
	typedef struct
	{
		__IO uint32_t MODER;    !< GPIO port mode register,               Address offset: 0x00     
		__IO uint32_t OTYPER;   !< GPIO port output type register,        Address offset: 0x04      
		__IO uint32_t OSPEEDR;  !< GPIO port output speed register,       Address offset: 0x08      
		__IO uint32_t PUPDR;    !< GPIO port pull-up/pull-down register,  Address offset: 0x0C      
		__IO uint32_t IDR;      !< GPIO port input data register,         Address offset: 0x10      
		__IO uint32_t ODR;      !< GPIO port output data register,        Address offset: 0x14      
		__IO uint32_t BSRR;     !< GPIO port bit set/reset register,      Address offset: 0x18      
		__IO uint32_t LCKR;     !< GPIO port configuration lock register, Address offset: 0x1C      
		__IO uint32_t AFR[2];   !< GPIO alternate function registers,     Address offset: 0x20-0x24 
	} GPIO_TypeDef;
	BSSR= Bit Set/Reset Register
	GPIOD base address: 0x40020C00
	BSRR offset: 0x18
	The BSRR register is divided into two 16-bit sections:
	Bits 0-15: The lower 16 bits (0-15) are used to set the corresponding GPIO pins.
	Bits 16-31: The upper 16 bits (16-31) are used to reset the corresponding GPIO pins.
	When you write a 1 to a bit in the lower 16 bits, the corresponding GPIO pin is set (turned on). 
	When you write a 1 to a bit in the upper 16 bits, the corresponding GPIO pin is reset (turned off).
	if you want to write the -> in another way it will be:
	(*GPIOD).BSSR
	reminder: if x=1024 -> (0...1000000000)b, z=x>>3 , z=128 ->  (0...0001000000)=2^7=128
	meaning shifting right is the same as division with 2's.
	shifting left is multiply by 2's
*/

#include "stm32f4xx.h"
#define LED_GREEN	 (1U<<12)
#define LED_ORANGE (1U<<13)
#define LED_RED		 (1U<<14)
#define LED_BLUE	 (1U<<15)

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
void turnOffAllLeds(){
	GPIOD->BSRR &=~LED_GREEN;
	GPIOD->BSRR &=~LED_ORANGE;
	GPIOD->BSRR &=~LED_RED;
	GPIOD->BSRR &=~LED_BLUE;
}
void ledSetUp(){
	// Configue LEDs
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // Enable the clock of port D of the GPIO
	
	GPIOD->MODER |= GPIO_MODER_MODER12_0; // Green LED, set pin 12 as output
	GPIOD->MODER |= GPIO_MODER_MODER13_0; // Orange LED, set pin 13 as output
	GPIOD->MODER |= GPIO_MODER_MODER14_0; // Red LED, set pin 14 as output
	GPIOD->MODER |= GPIO_MODER_MODER15_0; // Blue LED, set pin 15 as output
	// Turn off LEDs
	turnOffAllLeds();
}

int main(){
	ledSetUp();
	while(1){
		GPIOD->BSRR |= LED_GREEN; 
		delayfunc(1<<21);
		GPIOD->BSRR |= LED_ORANGE; 		
		delayfunc(1<<21);
		GPIOD->BSRR |= LED_RED; 		
		delayfunc(1<<21);
		GPIOD->BSRR |= LED_BLUE; 		
		delayfunc(1<<21);
		GPIOD->BSRR &=~LED_GREEN;
		delayfunc(1<<21);
		GPIOD->BSRR &=~LED_ORANGE;
		delayfunc(1<<21);
		GPIOD->BSRR &=~LED_RED; 
		delayfunc(1<<21);
		GPIOD->BSRR &=~LED_BLUE;
		delayfunc(1<<21);
	}
	return 0;
}

