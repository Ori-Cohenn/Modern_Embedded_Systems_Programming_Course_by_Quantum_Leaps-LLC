#ifndef BSP_H_
#define BSP_H_

#define BSP_TICKS_PER_SEC 100U

void SysTick_Handler(void);
void BSP_init(void);
int turnOffAllLeds();
void BSP_ledGreenOn(void);
void BSP_ledGreenOff(void);
void BSP_ledRedOn(void);
void BSP_ledRedOff(void);
void BSP_ledOrangeOn(void) ;
void BSP_ledOrangeOff(void) ;
void BSP_ledBlueOn(void) ;
void BSP_ledBlueOff(void);

void BSP_init(void);

/* get the current value of the clock tick counter (returns immedately) */
uint32_t BSP_tickCtr(void);

/* delay for a specified number of system clock ticks (polling) */
void BSP_delay(uint32_t ticks);

#endif //BSP_H_
