/*
 This lesson is about race condition of the green led from systick and orange led from the busy-loop.
 one of the solution is Mutual Exclusion (MUTEX)
 */
#include "stm32f4xx.h"
#include "bsp.h"

int main() {
	BSP_init();
//    BSP_ledGreenOn();
//    BSP_ledOrangeOn();
//	while (1) {
//		GPIOD->ODR |= (1U << LED_GREEN_PIN); // not atomic action (can be interrupted)
//		BSP_delay(1000000);
//		GPIOD->ODR &=~ (1U << LED_GREEN_PIN);
//		BSP_delay(1000000);
//
//	}
//	while (1) {
//		__disable_irq(); //Mutual Exclusion making them run on the same time!
//		GPIOD->ODR |= (1U << LED_GREEN_PIN); // not atomic action (can be interrupted)
//		BSP_delay(1000000);
//		__enable_irq();
//		__disable_irq();
//		GPIOD->ODR &= ~(1U << LED_GREEN_PIN);
//		BSP_delay(1000000);
//		__enable_irq();
//	}
	while (1) {
		GPIOD->BSRR = (1U << LED_GREEN_PIN); // atomic action
		BSP_delay(100000);
		GPIOD->BSRR = (1U << (LED_GREEN_PIN + 16U));
		BSP_delay(100000);
	}
return 0;
}
