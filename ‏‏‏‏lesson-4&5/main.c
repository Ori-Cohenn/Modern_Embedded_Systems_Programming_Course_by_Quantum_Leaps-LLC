/* the purpose of this lesson is blink the led
	memory model is in page 71 in data sheet SRAM is between 0x20...0 to 0x3fff...f
	Peripheral is 0x40000000 to 0x5FFFFFFF
	pd12 for led
	User LD3: orange LED is a user LED connected to the I/O PD13 of the
STM32F407VGT6 pin 60 *PD->GPIOD*
• User LD4: green LED is a user LED connected to the I/O PD12 of the
STM32F407VGT6 pin 59
• User LD5: red LED is a user LED connected to the I/O PD14 of the STM32F407VGT6.
pin61
• User LD6: blue LED is a user LED connected to the I/O PD15 of the STM32F407VGT6.
pin 62
0x40020C00 - 0x4002 0FFF GPIOD
The RCC_AHB1ENR (RCC AHB1 peripheral clock enable register) enables or disables the clocks for the peripherals connected to the AHB1 bus.
When you set a particular bit in this register, you enable the clock for the corresponding peripheral, allowing it to operate.
The base address of the RCC peripheral is 0x40023800. The RCC_AHB1ENR register is located at an offset of 0x30 from the base address. 
Thus, its address is 0x40023830.
GPIOD_EN (GPIO Port D clock enable)-> 2^3=8->> 0x00000008 to enable GPIOD
GPIOx_MODER-> 01 to output, 0x40020C00 to 0x01000000 for output PD13
GPIOx_ODR sets value output 0x40020C14 (14 is offset) to value 2000 ->0010 0000 0000 0000 (13th pin) for ld3
for ld6-> pd15-> 0x8000 ->1000 0000 0000 0000
*/
/*volatile prefix is for telling the compiler that the variable can be changed even through we did not write it in code
by that priventing it to optimize it to none-volotile- "static"
this is very useful for i\o register, push buttons and other perprials*/

#define RCC_AHB1ENR 0x40023830 //adress
#define GPIOD_EN 0x00000008 
#define GPIOD_MODER 0x40020C00
#define PD15_OUTPUT 0x55000000
#define GPIOD_ODR 0x40020C14
#define LED6_ON_blue 0x00008000
#define LED5_ON_red 0x00004000
#define LED4_ON_orange 0x00002000
#define LED3_ON_green 0x00001000
#define LEDs_ON 0x0000F000
#define LEDs_OFF 0x0000

int delayfunc(int delay){
	for(volatile int i=0;i<delay;i++) __asm("NOP");
	return 0;
}

int main() {
    // Enable the clock for GPIOD
    *((volatile unsigned int *) RCC_AHB1ENR) = GPIOD_EN;
    
    // Set PD15 as output
    *((volatile unsigned int *) GPIOD_MODER) = PD15_OUTPUT;
    *((volatile unsigned int *) GPIOD_ODR)&=LEDs_OFF;
    // Turn on LEDs (PD)
    *((volatile unsigned int *) GPIOD_ODR) |= LEDs_ON;
		while(1){
			delayfunc((1<<20));
			*((volatile unsigned int *) GPIOD_ODR)&=LEDs_OFF;
			delayfunc((1<<20));
			*((volatile unsigned int *) GPIOD_ODR)|=LED6_ON_blue;
			delayfunc(1<<20);
			*((volatile unsigned int *) GPIOD_ODR)|=LED5_ON_red;
			delayfunc(1<<20);
			*((volatile unsigned int *) GPIOD_ODR)|=LED4_ON_orange;
			delayfunc(1<<20);
			*((volatile unsigned int *) GPIOD_ODR)|=LED3_ON_green;
			delayfunc(1<<20);
		}
    return 0;
}
