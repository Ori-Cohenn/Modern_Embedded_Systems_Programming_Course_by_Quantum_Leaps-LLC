/*****************************************************************************
* BSP for STM32F4-Discovery
*****************************************************************************/
#include "bsp.h"

//static uint32_t volatile is_pressed = 0;


void BSP_init(void)
{										 
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // enable GPIOD clock port for the LEDs and
	// set the LED pins as push-pull output, no pull-up,medium speed, pull-down
	GPIOD->MODER &= ~((3U << 2U*LED_GREEN_PIN)|(3U << 2U*LED_ORANGE_PIN)|
	(3U << 2U*LED_RED_PIN)|(3U << 2U*LED_BLUE_PIN));	
	GPIOD->MODER |= (GPIO_MODER_MODER12_0 | GPIO_MODER_MODER13_0 | GPIO_MODER_MODER14_0 | GPIO_MODER_MODER15_0); // Green LED, set pin 12 as output
	GPIOD->OTYPER &= ~((1U << LED_GREEN_PIN) | (1U << LED_RED_PIN) | (1U << LED_ORANGE_PIN) | (1U << LED_BLUE_PIN));
	GPIOD->OSPEEDR &= ~((3U << 2U * LED_GREEN_PIN) | (3U << 2U * LED_RED_PIN) | (3U << 2U * LED_ORANGE_PIN) | (3U << 2U * LED_BLUE_PIN));
	GPIOD->OSPEEDR |= ((1U << 2U * LED_GREEN_PIN) | (1U << 2U * LED_RED_PIN) 
	| (1U << 2U * LED_ORANGE_PIN) | (1U << 2U * LED_BLUE_PIN));
	GPIOD->PUPDR &= ~((3U << 2U * LED_GREEN_PIN) | (3U << 2U * LED_RED_PIN) | (3U << 2U * LED_ORANGE_PIN) | (3U << 2U * LED_BLUE_PIN));

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; //enable GPIOA clock port for button

	
	GPIOA->MODER &= ~((3U << 2U*BTN_SW1) | (3U << 2U*BTN_SW2)); // Set PA1 as input-digital for analog moder=11
	GPIOA->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR0);//Low speed
    GPIOA->PUPDR &= ~((3U << 2U*BTN_SW1) | (3U << 2U*BTN_SW2));  // Clear previous pull configuration
	GPIOA->PUPDR |= ((2U << 2U * BTN_SW1) | (2U << 2U * BTN_SW2));    // Set pull-down resistor on PA1 as it is in stm32f4 Discovery Board
	
	
	// Enable the SYSCFG clock for EXTI configuration
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	
	
	    // Connect PA1 to EXTI1 line (EXTI1 is mapped to PA1)
    SYSCFG->EXTICR[0] &= ~((0xFU << 4U)|(0xFU << 0U)); // Clear EXTI1 bits
    SYSCFG->EXTICR[0] |= ((0x0U << 4U) | (0x0U << 0U)); // Set EXTI1 to use PA1

    // Configure EXTI1 for falling edge trigger (button press detection)
    EXTI->FTSR |= ((1U << BTN_SW2)|(1U << BTN_SW1));  // Falling edge trigger enabled
    EXTI->RTSR &= ~((1U << BTN_SW2)|(1U << BTN_SW1)); // Disable rising edge trigger

    // Enable EXTI1 interrupt
    EXTI->IMR |= ((1U << BTN_SW2)|(1U << BTN_SW1));  // Unmask EXTI1

    // Enable EXTI1 interrupt in NVIC
//    NVIC_EnableIRQ(EXTI1_IRQn);
//    NVIC_SetPriority(EXTI1_IRQn, 2U); // Set priority (0 = highest, 15 = lowest)

}


void on_when_press(void){
	volatile uint32_t SW1_is_press = 0U;
	volatile uint32_t SW2_is_press = 0U;
	volatile uint32_t one_of_them_press = 0U;
	SW1_is_press = (GPIOA->IDR & (1 << BTN_SW1));
	SW2_is_press = (GPIOA->IDR & (1 << BTN_SW2));
	one_of_them_press = (GPIOA->IDR & (1 << BTN_SW2)) | (GPIOA->IDR & (1 << BTN_SW1));
	if(SW2_is_press){
		GPIOD->BSRR = (1U << LED_GREEN_PIN);		
	}
	if(!SW2_is_press){
//	is_pressed = 0;
	GPIOD->BSRR = (1U << (LED_GREEN_PIN + 16U)); 
	}
	
	if(SW1_is_press){
		GPIOD->BSRR = (1U << LED_RED_PIN);		
	}
	if(!SW1_is_press){
//	is_pressed = 0;
	GPIOD->BSRR = (1U << (LED_RED_PIN + 16U)); 
	}
	if(one_of_them_press){
		GPIOD->BSRR = (1U << LED_ORANGE_PIN);
	}
	if(!one_of_them_press){
		GPIOD->BSRR = (1U << (LED_ORANGE_PIN + 16U));
	}
	
}

//// EXTI1 Interrupt Handler
//void EXTI1_IRQHandler(void) {
//    if (EXTI->PR & (1 << BTN_SW2)) {  // Check if interrupt was triggered by PA1 (BTN_SW2)
//		if (!(GPIOA->IDR & (1 << BTN_SW2))){
//			is_pressed = 1;
//		}
//		EXTI->PR |= (1 << BTN_SW2);  // Clear EXTI1 interrupt flag
//	}

//}