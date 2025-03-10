/*****************************************************************************
* BSP for STM32F4-Discovery with with QP/C framework
*****************************************************************************/
#include "qpc.h"  /* uC/AO API */
#include "bsp.h"
#include "stm32f4xx.h"


#define LED_GREEN_PIN 12U
#define LED_ORANGE_PIN 13U
#define LED_RED_PIN 14U
#define LED_BLUE_PIN 15U
#define TEST_PIN  5U
#define SYS_CLOCK_HZ (1U << 24)

// Button B1 (PA.0)
#define BTN_SW1   0U // has pull-down res
 // external buttun connected to PA.1
#define BTN_SW2   1U


/* ISRs ===============================================*/
void SysTick_Handler(void) {
    /* state of the button debouncing, see below */
    static struct ButtonsDebouncing {
        uint32_t depressed;
        uint32_t previous;
    } buttons = { 0U, 0U };
    uint32_t current;
    uint32_t tmp;
	
    QF_TICK_X(0U, (void *)0); /* process all QP/C time events */

    current = (GPIOA->IDR & (1 << BTN_SW2)) | (GPIOA->IDR & (1 << BTN_SW1)); // read Port A with state of Button BTN_SW1 and BTN_SW2
    tmp = buttons.depressed; /* save the debounced depressed buttons */
    buttons.depressed |= (buttons.previous & current); /* set depressed */
    buttons.depressed &= (buttons.previous | current); /* clear released */
    buttons.previous  = current; /* update the history */
    tmp ^= buttons.depressed;     /* changed debounced depressed */
    if ((tmp & (1U << BTN_SW1)) != 0U) {  /* debounced SW1 state changed? */
        if ((buttons.depressed & (1U << BTN_SW1)) != 0U) { /* is SW1 depressed? */
            /* post the "button-pressed" event from ISR */
			static QEvt const buttonPressedEvt
                              = QEVT_INITIALIZER(BUTTON_PRESSED_SIG);
            QACTIVE_POST(AO_TimeBomb, &buttonPressedEvt, 0U);
        }
        else { /* the button is released */
            /* post the "button-released" event from ISR */
            static QEvt const buttonReleasedEvt
                              = QEVT_INITIALIZER(BUTTON_RELEASED_SIG);
            QACTIVE_POST(AO_TimeBomb, &buttonReleasedEvt, 0U);
        }
    }
    if ((tmp & (1U << BTN_SW2)) != 0U) {  /* debounced SW2 state changed? */
        if ((buttons.depressed & (1U << BTN_SW2)) != 0U) { /* is SW2 depressed? */
            /* post the "button-pressed" event from ISR */
            static QEvt const button2PressedEvt
                              = QEVT_INITIALIZER(BUTTON2_PRESSED_SIG);
            QACTIVE_POST(AO_TimeBomb, &button2PressedEvt, 0U);
        }
        else { /* the button is released */
            /* post the "button-released" event from ISR */
            static QEvt const button2ReleasedEvt
                              = QEVT_INITIALIZER(BUTTON2_RELEASED_SIG);
            QACTIVE_POST(AO_TimeBomb, &button2ReleasedEvt, 0U);
        }
    }
}
/*..........................................................................*/

void QV_onIdle(void) {
#ifdef NDEBUG
    /* Put the CPU and peripherals to the low-power mode.
    * you might need to customize the clock management for your application,
    * see the datasheet for your particular Cortex-M MCU.
    */
    QV_CPU_SLEEP();  /* atomically go to sleep and enable interrupts */
#else
    QF_INT_ENABLE(); /* just enable interrupts */
#endif
}


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
	

}

void QF_onStartup(void) {
    /* set up the SysTick timer to fire at BSP_TICKS_PER_SEC rate
    * NOTE: do NOT call OS_CPU_SysTickInit() from uC/OS-II
    */
    SysTick_Config(SystemCoreClock / BSP_TICKS_PER_SEC);

    /* set priorities of ALL ISRs used in the system, see NOTE1 */
    NVIC_SetPriority(SysTick_IRQn,  QF_AWARE_ISR_CMSIS_PRI + 1U);
    /* ... */

    /* enable IRQs in the NVIC... */
    /* ... */
}

/*..........................................................................*/
void QF_onCleanup(void) {
}

/*..........................................................................*/

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

