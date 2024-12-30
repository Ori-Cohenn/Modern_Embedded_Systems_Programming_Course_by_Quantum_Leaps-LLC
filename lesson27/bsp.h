#ifndef BSP_H_
#define BSP_H_

/* system clock tick [Hz] */
#define BSP_TICKS_PER_SEC 1000U

void SysTick_Handler(void);
void BSP_init(void);
int turnOffAllLeds();
void BSP_ledGreenOn(void);
void BSP_ledGreenOff(void);
void BSP_ledRedOn(void);
void BSP_ledRedOff(void);
void BSP_ledOrangeOn(void);
void BSP_ledOrangeOff(void);
void BSP_ledBlueOn(void);
void BSP_ledBlueOff(void);
void BSP_TestPinAOn(void);
void BSP_TestPinAOff(void);

extern QXSemaphore SW1_sema;


#endif // BSP_H_
