#ifndef BSP_H_
#define BSP_H_

void SysTick_Handler(void);
int turnOffAllLeds();
void BSP_init(void);
void BSP_ledGreenOn(void);
void BSP_ledGreenOff(void);
void BSP_ledRedOn(void);
void BSP_ledRedOff(void);
void BSP_ledOrangeOn(void) ;
void BSP_ledOrangeOff(void) ;
void BSP_ledBlueOn(void) ;
void BSP_ledBlueOff(void);
void BSP_delay(int volatile iter);

#endif //BSP_H_
