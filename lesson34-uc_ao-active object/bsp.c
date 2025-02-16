/*****************************************************************************
* BSP for STM32F4-Discovery with uC/AO active-object framework
*****************************************************************************/
#include "uc_ao.h"  /* uC/AO API */
#include "qassert.h"  /* embedded-system-friendly assertions */
#include "bsp.h"
#include "stm32f4xx.h"


#define LED_GREEN_PIN 12U
#define LED_ORANGE_PIN 13U
#define LED_RED_PIN 14U
#define LED_BLUE_PIN 15U
#define TEST_PIN  5U
#define SYS_CLOCK_HZ (1U << 24)

// Button B1 (PA.0)
#define B1_PIN   0U // has pull-down res



/* uCOS-II application hooks ===============================================*/
void App_TimeTickHook(void) {
    /* state of the button debouncing, see below */
    static struct ButtonsDebouncing {
        uint32_t depressed;
        uint32_t previous;
    } buttons = { 0U, 0U };
    uint32_t current;
    uint32_t tmp;
	
	TimeEvent_tick(); /* process all uC/AO time events */


    /* Perform the debouncing of buttons. The algorithm for debouncing
    * adapted from the book "Embedded Systems Dictionary" by Jack Ganssle
    * and Michael Barr, page 71.
    */
    current = ~GPIOA->IDR; // read Port A with state of Button B1
    tmp = buttons.depressed; /* save the debounced depressed buttons */
    buttons.depressed |= (buttons.previous & current); /* set depressed */
    buttons.depressed &= (buttons.previous | current); /* clear released */
    buttons.previous   = current; /* update the history */
    tmp ^= buttons.depressed;     /* changed debounced depressed */
    if ((tmp & (1U << B1_PIN)) != 0U) { // debounced B1 state changed?
        if ((current & (1U << B1_PIN)) != 0U) { // is B1 depressed?
            /* post the "button-pressed" event */
            static Event const buttonPressedEvt = {BUTTON_PRESSED_SIG};
            Active_post(AO_BlinkyButton, &buttonPressedEvt);
        }
        else { /* the button is released */
            /* post the "button-released" event */
            static Event const buttonReleasedEvt = {BUTTON_RELEASED_SIG};
            Active_post(AO_BlinkyButton, &buttonReleasedEvt);
        }
    }
}
/*..........................................................................*/

void App_TaskIdleHook(void) {
#ifdef NDEBUG
    /* Put the CPU and peripherals to the low-power mode.
    * you might need to customize the clock management for your application,
    * see the datasheet for your particular Cortex-M4 MCU.
    */
    __WFI(); /* Wait-For-Interrupt */
#endif
}
/*..........................................................................*/
void App_TaskCreateHook (OS_TCB *ptcb) { (void)ptcb; }
void App_TaskDelHook    (OS_TCB *ptcb) { (void)ptcb; }
void App_TaskReturnHook (OS_TCB *ptcb) { (void)ptcb; }
void App_TaskStatHook   (void)         {}
void App_TaskSwHook     (void)         {}
void App_TCBInitHook    (OS_TCB *ptcb) { (void)ptcb; }

/* BSP functions ===========================================================*/

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
	
    // configure Button B1 (PA.0) pins as input, no pull-up, pull-down
    GPIOA->MODER &= ~(3U << 2U*B1_PIN); // Set PA0 as input-digital for analog moder=11
	GPIOA->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR0);//Low speed
    GPIOA->PUPDR &= ~(3U << 2U*B1_PIN);  // Clear previous pull configuration
	GPIOA->PUPDR |= (2U << 2U * B1_PIN);    // Set pull-down resistor on PA0 as it is in stm32f4 Discovery Board
	
	// Enable the SYSCFG clock for EXTI configuration
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	
    // Configure EXTI Line 0 for falling edge trigger on PA0 
	// EXTI Line 0: Pin 0 of any GPIO port (e.g., PA0, PB0, PC0, etc.)
    SYSCFG->EXTICR[0] &= ~(0xFU << 0U); // Clear EXTI0[0]
    SYSCFG->EXTICR[0] |= (0x0U << 0U);  // Map EXTI0 to GPIOA (Port A)

    EXTI->IMR |= (1U << B1_PIN);   // Enable interrupt request for EXTI0
    EXTI->FTSR |= (1U << B1_PIN);  // Enable falling edge trigger
    EXTI->RTSR &= ~(1U << B1_PIN); // Disable rising edge trigger
}

void BSP_start(void) {
    /* NOTE: SystemInit() has been already called from the startup code
    *  but SystemCoreClock needs to be updated
    */
    SystemCoreClockUpdate();

    /* set up the SysTick timer to fire at BSP_TICKS_PER_SEC rate
    * NOTE: do NOT call OS_CPU_SysTickInit() from uC/OS-II
    */
    SysTick_Config(SystemCoreClock / OS_TICKS_PER_SEC);

    /* set priorities of ALL ISRs used in the system, see NOTE1 */
    NVIC_SetPriority(SysTick_IRQn,  CPU_CFG_KA_IPL_BOUNDARY + 1U);
    /* ... */

    /* enable IRQs in the NVIC... */
    /* ... */
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

//............................................................................
Q_NORETURN Q_onAssert(char const * const module, int const id) {
    (void)module; // unused parameter
    (void)id;     // unused parameter
#ifndef NDEBUG
    GPIOD->BSRR = (1U << LED_RED_PIN);  // turn LED on
    // for debugging, hang on in an endless loop...
    for (;;) {
    }
#endif
    NVIC_SystemReset();
}
// error-handling function called by exception handlers in the startup code
Q_NORETURN assert_failed(char const * const module, int const id);
Q_NORETURN assert_failed(char const * const module, int const id) {
    Q_onAssert(module, id);
}

