/*****************************************************************************
* BSP for STM32F4-Discovery with uC/OS-II RTOS
*****************************************************************************/
#ifndef BSP_H_
#define BSP_H_


void BSP_init(void);
void BSP_start(void);
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

/* global RTOS objects... */
extern OS_EVENT *BSP_semaPress;   /* global semaphore handle */
extern OS_EVENT *BSP_semaRelease; /* global semaphore handle */


#endif // BSP_H_
