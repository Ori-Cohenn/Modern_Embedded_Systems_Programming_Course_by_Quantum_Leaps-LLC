/*****************************************************************************
* BSP for STM32F4-Discovery
*****************************************************************************/
#ifndef BSP_H_
#define BSP_H_
#include "stm32f4xx.h"
#define LED_GREEN_PIN 12U
#define LED_ORANGE_PIN 13U
#define LED_RED_PIN 14U
#define LED_BLUE_PIN 15U

// Button B1 (PA.0)
#define BTN_SW1   0U // has pull-down res
// Button B2 (PA.1)
#define BTN_SW2   1U


void BSP_init(void);
void EXTI1_IRQHandler(void);
void on_when_press(void);


#endif // BSP_H_
