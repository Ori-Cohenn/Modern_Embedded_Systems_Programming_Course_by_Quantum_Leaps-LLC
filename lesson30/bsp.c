#include "qpc.h"
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

// Semaphore for the QXK kernel
extern QXSemaphore SW1_sema;

//SEMA
//static QXSemaphore Morse_sema;

//MUTEX
static QXMutex Morse_mutex;

void SysTick_Handler(void) { // irq in cortexm4 header file (CMSIS)
	
    QXK_ISR_ENTRY();  /* inform QXK about entering an ISR */

    QF_TICK_X(0U, (void *)0); /* process time events for rate 0 */

    QXK_ISR_EXIT(); /* inform QXK about exiting an ISR */
}

void EXTI0_IRQHandler(void); // prototype
void EXTI0_IRQHandler(void) { //EXTI stands for External Interrupt/Event Controller
    QXK_ISR_ENTRY();  /* inform QXK about entering an ISR */

    /* falling edge? */
    if ((EXTI->PR & (1U << B1_PIN))!= 0U) {
        EXTI->PR |= (1U << B1_PIN); /* clear interrupt */
        QXSemaphore_signal(&SW1_sema);
    }

    QXK_ISR_EXIT(); /* inform QXK about exiting an ISR */
}

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
	   //SEMA
    ///* initialize the Morse_sema semaphore as binary, resource semaphore */
    //QXSemaphore_init(&Morse_sema, /* pointer to semaphore to initialize */
    //                 1U,  /* initial semaphore count (resource semaphore) */
    //                 1U); /* maximum semaphore count (binary semaphore) */

    //MUTEX
    QXMutex_init(&Morse_mutex, 6U); /* priority ceiling 6 */
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
void BSP_ledGreenToggle(void) // using the same register to represent shared register->lesson28
{
	//CRIT safe after reading, eliminate race condition
    QF_CRIT_STAT

    QF_CRIT_ENTRY();
	GPIOD->ODR ^= LED_GREEN_PIN;
	QF_CRIT_EXIT();	
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
void BSP_ledRedToggle(void)
{
	//CRIT safe after reading
    QF_CRIT_STAT

    QF_CRIT_ENTRY();
	GPIOD->ODR ^= LED_RED_PIN;
	QF_CRIT_EXIT();	
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
void BSP_ledOrangeToggle(void)
{
	//CRIT safe after reading
    QF_CRIT_STAT

    QF_CRIT_ENTRY();
	GPIOD->ODR ^= LED_ORANGE_PIN;
	QF_CRIT_EXIT();	
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
void BSP_ledBlueToggle(void)
{
	//CRIT safe after reading
    QF_CRIT_STAT

    QF_CRIT_ENTRY();
	GPIOD->ODR ^= LED_BLUE_PIN;
	QF_CRIT_EXIT();	
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

void BSP_sendMorseCode(uint32_t bitmask) {
    uint32_t volatile delay_ctr;
    enum { DOT_DELAY = 150 };

    //LOCK
    //QSchedStatus sstat;

    //SEMA
    //QXSemaphore_wait(&Morse_sema,  /* pointer to semaphore to wait on */
    //                 QXTHREAD_NO_TIMEOUT); /* timeout for waiting */

    //LOCK
    //sstat = QXK_schedLock(5U); /* priority ceiling 5 */

    //MUTEX
    QXMutex_lock(&Morse_mutex,
                 QXTHREAD_NO_TIMEOUT); /* timeout for waiting */

    for (; bitmask != 0U; bitmask <<= 1) {
        if ((bitmask & (1U << 31)) != 0U) {
            BSP_ledGreenOn();
        }
        else {
            BSP_ledGreenOff();
        }
        for (delay_ctr = DOT_DELAY;
             delay_ctr != 0U; --delay_ctr) {
        }
    }
    BSP_ledGreenOff();
    for (delay_ctr = 7*DOT_DELAY;
         delay_ctr != 0U; --delay_ctr) {
    }

    //SEMA
    //QXSemaphore_signal(&Morse_sema);  /* pointer to semaphore to signal */

    //LOCK
    //QXK_schedUnlock(sstat);

    //MUTEX
    QXMutex_unlock(&Morse_mutex);
}



void QF_onStartup(void) {
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / BSP_TICKS_PER_SEC);

    /* set the interrupt priorities of "kernel aware" interrupts */
    NVIC_SetPriority(SysTick_IRQn, 0U);
	NVIC_SetPriority(EXTI0_IRQn,  0U);
	
	/* enable interrupts */
    NVIC_EnableIRQ(EXTI0_IRQn);

}

/*..........................................................................*/
void QF_onCleanup(void) {
}
/*..........................................................................*/


void QXK_onIdle(void) {
	GPIOD->BSRR = (1U << LED_BLUE_PIN);
	GPIOD->BSRR = (1U << (LED_BLUE_PIN + 16U));
	#ifdef NDBEBUG
		__WFI(); /* stop the CPU and Wait for Interrupt */
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

