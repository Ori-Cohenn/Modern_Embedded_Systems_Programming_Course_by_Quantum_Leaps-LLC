#include "stm32f4xx.h"
#include "bsp.h"

void SysTick_Handler(void) { //irq in cortexm4 header file (CMSIS)
//		GPIOD->ODR ^= (1U << LED_ORANGE_PIN);
    if (GPIOD->IDR & (1U << LED_ORANGE_PIN)) { // is LED_GREEN_PIN ON?
        GPIOD->BSRR = (1U << (LED_ORANGE_PIN + 16U)); // turn LED_GREEN_PIN ON
    }
    else { // LED_GREEN_PIN is OFF
        GPIOD->BSRR = (1U << LED_ORANGE_PIN); // turn LED_GREEN_PIN on
    }
}

void BSP_init(void) {
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // enable GPIOD clock port for the LEDs
	// set the LED pins as push-pull output, no pull-up,medium speed, pull-down
	GPIOD->MODER = (GPIO_MODER_MODER12_0 | GPIO_MODER_MODER13_0
			| GPIO_MODER_MODER14_0 | GPIO_MODER_MODER15_0); // Green LED, set pin 12 as output
	GPIOD->OTYPER &= ~((1U << LED_GREEN_PIN) | (1U << LED_RED_PIN)
			| (1U << LED_ORANGE_PIN) | (1U << LED_BLUE_PIN));
	GPIOD->OSPEEDR |= ((1U << 2U * LED_GREEN_PIN) | (1U << 2U * LED_RED_PIN)
			| (1U << 2U * LED_ORANGE_PIN) | (1U << 2U * LED_BLUE_PIN));
	GPIOD->PUPDR &= ~((3U << 2U * LED_GREEN_PIN) | (3U << 2U * LED_RED_PIN)
			| (3U << 2U * LED_ORANGE_PIN) | (3U << 2U * LED_BLUE_PIN));

	// configure the SysTick interrupt
	SysTick->LOAD = SYS_CLOCK_HZ / 2U - 1U; //divider 8M/16Mhz = 0.5 Second (count to to 8M in a 16Mhz timer)
	SysTick->VAL = 0U;
	SysTick->CTRL = (1U << 2U) | (1U << 1U) | 1U;
	__enable_irq();

}

void BSP_ledGreenOn(void) {
	GPIOD->BSRR = (1U << LED_GREEN_PIN);
}
//............................................................................
void BSP_ledGreenOff(void) {
	GPIOD->BSRR = (1U << (LED_GREEN_PIN + 16U));
}
//............................................................................
void BSP_ledRedOn(void) {
	GPIOD->BSRR = (1U << LED_RED_PIN);
}
//............................................................................
void BSP_ledRedOff(void) {
	GPIOD->BSRR = (1U << (LED_RED_PIN + 16U));
}
//............................................................................
void BSP_ledOrangeOn(void) {
	GPIOD->BSRR = (1U << LED_ORANGE_PIN);
}
//............................................................................
void BSP_ledOrangeOff(void) {
	GPIOD->BSRR = (1U << (LED_ORANGE_PIN + 16U));
}
//............................................................................

void BSP_ledBlueOn(void) {
	GPIOD->BSRR = (1U << LED_BLUE_PIN);
}
//............................................................................
void BSP_ledBlueOff(void) {
	GPIOD->BSRR = (1U << (LED_BLUE_PIN + 16U));
}
//............................................................................

void BSP_delay(int volatile iter) {
	while (iter > 0) {  // delay loop
		--iter;
	}
}

int turnOffAllLeds(void) {
	GPIOD->BSRR = 1 << (12 + 16); // Set the BSRR bit 12 + 16->F to 1 to turn respective LED off
	GPIOD->BSRR = 1 << (13 + 16); // Set the BSRR bit 13 + 16 to 1 to turn respective LED off
	GPIOD->BSRR = 1 << (14 + 16); // Set the BSRR bit 14 + 16 to 1 to turn respective LED off
	GPIOD->BSRR = 1 << (15 + 16); // Set the BSRR bit 15 + 16 to 1 to turn respective LED off
	return 0;
}

void assert_failed(char const *const module, int const id); // prototype
void assert_failed(char const *const module, int const id) {
	(void) module; // unused parameter
	(void) id;     // unused parameter
#ifndef NDEBUG
	// light up the user LED
	GPIOA->BSRR = (1U << LED_RED_PIN);  // turn LED on
	// for debugging, hang on in an endless loop...
	for (;;) {
	}
#endif

	NVIC_SystemReset();
}

