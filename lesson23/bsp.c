#include <stdint.h>  /* Standard integers. WG14/N843 C99 Standard */
#include "bsp.h"
#include "stm32f4xx.h"
#include "miros.h"

#define LED_GREEN_PIN	 12U
#define LED_ORANGE_PIN 	 13U
#define LED_RED_PIN		 14U
#define LED_BLUE_PIN	 15U
#define SYS_CLOCK_HZ 	(1U<<24)


static uint32_t volatile l_tickCtr;

void SysTick_Handler(void) { //irq in cortexm4 header file (CMSIS)
    ++l_tickCtr;
	
		__disable_irq();
		OS_sched();
		__enable_irq();
}

void BSP_init(void) { //bsp-board support package
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

    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock/BSP_TICKS_PER_SEC);
	
	/* set the Systick interrupt priority (highest)*/
	NVIC_SetPriority(SysTick_IRQn,0U); //0U highest priority
	
	
	__enable_irq();

}


uint32_t BSP_tickCtr(void) {
    uint32_t tickCtr;

    __disable_irq(); //critical section
    tickCtr = l_tickCtr;
    __enable_irq();

    return tickCtr;
}

void BSP_delay(uint32_t ticks) {
    uint32_t start = BSP_tickCtr();
    while ((BSP_tickCtr() - start) < ticks) {
    }
}

int turnOffAllLeds(void) {
	GPIOD->BSRR = 1 << (12 + 16); // Set the BSRR bit 12 + 16->F to 1 to turn respective LED off
	GPIOD->BSRR = 1 << (13 + 16); // Set the BSRR bit 13 + 16 to 1 to turn respective LED off
	GPIOD->BSRR = 1 << (14 + 16); // Set the BSRR bit 14 + 16 to 1 to turn respective LED off
	GPIOD->BSRR = 1 << (15 + 16); // Set the BSRR bit 15 + 16 to 1 to turn respective LED off
	return 0;
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

_Noreturn void assert_failed(char const * const module, int const id);
_Noreturn void assert_failed(char const * const module, int const id) {
    (void)module; // unused parameter
    (void)id;     // unused parameter
#ifndef NDEBUG
	// light up the user LEDs
	GPIOD->BSRR = (1U << LED_RED_PIN);  
	GPIOD->BSRR = (1U << LED_GREEN_PIN);
	GPIOD->BSRR = (1U << LED_ORANGE_PIN);
	GPIOD->BSRR = (1U << LED_BLUE_PIN);
	// for debugging, hang on in an endless loop...
	for (;;) {
	}
#endif
	NVIC_SystemReset();
}

