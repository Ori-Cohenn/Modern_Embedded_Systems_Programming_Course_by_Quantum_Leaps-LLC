#include <stdint.h> /* Standard integers. WG14/N843 C99 Standard */
#include "bsp.h"
#include "stm32f4xx.h"
#include "miros.h"

#define LED_GREEN_PIN 12U
#define LED_ORANGE_PIN 13U
#define LED_RED_PIN 14U
#define LED_BLUE_PIN 15U
#define TEST_PIN  5U
#define SYS_CLOCK_HZ (1U << 24)

void SysTick_Handler(void)
{ // irq in cortexm4 header file (CMSIS)
//	BSP_TestPinAOn();
	
	OS_tick();

	__disable_irq();
	OS_sched();
	__enable_irq();
	
//	BSP_TestPinAOff();
}
//| RCC_AHB1ENR_GPIOAEN;
void BSP_init(void)
{										 // bsp-board support package
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;// enable GPIOD clock port for the LEDs and GPIOA for testing
	// set the LED pins as push-pull output, no pull-up,medium speed, pull-down
	GPIOD->MODER = (GPIO_MODER_MODER12_0 | GPIO_MODER_MODER13_0 | GPIO_MODER_MODER14_0 | GPIO_MODER_MODER15_0); // Green LED, set pin 12 as output
	GPIOD->OTYPER &= ~((1U << LED_GREEN_PIN) | (1U << LED_RED_PIN) | (1U << LED_ORANGE_PIN) | (1U << LED_BLUE_PIN));
	GPIOD->OSPEEDR |= ((3U << 2U * LED_GREEN_PIN) | (3U << 2U * LED_RED_PIN) | (3U << 2U * LED_ORANGE_PIN) | (3U << 2U * LED_BLUE_PIN));
	GPIOD->PUPDR &= ~((3U << 2U * LED_GREEN_PIN) | (3U << 2U * LED_RED_PIN) | (3U << 2U * LED_ORANGE_PIN) | (3U << 2U * LED_BLUE_PIN));
//	//Configure PA5 as push-pull output for the test pin
//	GPIOA->MODER = GPIO_MODER_MODER5_0;      // Set PA5 as output
//	GPIOA->OTYPER &= ~(1U << TEST_PIN);        // Push-pull output
//	GPIOA->OSPEEDR |= (1U << (2U * TEST_PIN)); // Medium speed (you can adjust speed as needed)
//	GPIOA->PUPDR &= ~(3U << 2U * TEST_PIN);      // No pull-up/down

}

int turnOffAllLeds(void)
{
	GPIOD->BSRR = 1 << (12 + 16); // Set the BSRR bit 12 + 16->F to 1 to turn respective LED off
	GPIOD->BSRR = 1 << (13 + 16); // Set the BSRR bit 13 + 16 to 1 to turn respective LED off
	GPIOD->BSRR = 1 << (14 + 16); // Set the BSRR bit 14 + 16 to 1 to turn respective LED off
	GPIOD->BSRR = 1 << (15 + 16); // Set the BSRR bit 15 + 16 to 1 to turn respective LED off
	return 0;
}

void BSP_ledGreenOn(void)
{
	GPIOD->BSRR = (1U << LED_GREEN_PIN);
}
//............................................................................
void BSP_ledGreenOff(void)
{
	GPIOD->BSRR = (1U << (LED_GREEN_PIN + 16U));
}
//............................................................................
void BSP_ledRedOn(void)
{
	GPIOD->BSRR = (1U << LED_RED_PIN);
}
//............................................................................
void BSP_ledRedOff(void)
{
	GPIOD->BSRR = (1U << (LED_RED_PIN + 16U));
}
//............................................................................
void BSP_ledOrangeOn(void)
{
	GPIOD->BSRR = (1U << LED_ORANGE_PIN);
}
//............................................................................
void BSP_ledOrangeOff(void)
{
	GPIOD->BSRR = (1U << (LED_ORANGE_PIN + 16U));
}
//............................................................................

void BSP_ledBlueOn(void)
{
	GPIOD->BSRR = (1U << LED_BLUE_PIN);
}
//............................................................................
void BSP_ledBlueOff(void)
{
	GPIOD->BSRR = (1U << (LED_BLUE_PIN + 16U));
}
//............................................................................
void BSP_TestPinAOn(void)
{
	GPIOA->BSRR = (1U <<TEST_PIN);
}
//............................................................................
void BSP_TestPinAOff(void)
{
	GPIOA->BSRR = (1U << (TEST_PIN + 16U));
}
//............................................................................



void OS_onStartup(void) {
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / BSP_TICKS_PER_SEC);

    /* set the SysTick interrupt priority (highest) */
    NVIC_SetPriority(SysTick_IRQn, 0U);
	
}


void OS_onIdle(void) {
#ifdef NDBEBUG
    __WFI(); /* stop the CPU and Wait for Interrupt low power mode */
#endif
}

void Q_onAssert(char const *module, int id) {
    /* TBD: damage control */
    (void)module; /* avoid the "unused parameter" compiler warning */
    (void)id;     /* avoid the "unused parameter" compiler warning */
    NVIC_SystemReset();
}

void assert_failed(char const *module, int id) {
    Q_onAssert(module, id);
}

