/*****************************************************************************
* BSP for STM32F4-Discovery with uC/AO active-object framework
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

/*---------------------------------------------------------------------------*/
enum EventSignals {
    BUTTON_PRESSED_SIG = USER_SIG,
    BUTTON_RELEASED_SIG,
    TIMEOUT_SIG,
    /* ... */
	MAX_SIG
};

/* Active objects... */
extern Active *AO_TimeBomb;

#endif // BSP_H_
