#ifndef __FUNCTIONS__
#define __FUNCTIONS__

#define myGPIOD_base	 0x40020C00				
#define LED_GREEN	 		(1UL<<12)
#define LED_GREEN_OFF	(1U<<(12+16))
#define LED_ORANGE 		(1U<<13)
#define LED_RED		 		(1U<<14)
#define LED_RED_OFF		(1U<<(14+16))
#define LED_BLUE	 		(1U<<15)
#define LED_BLUE_OFF	(1U<<(15+16))

int delayfunc(int volatile delay);
int turnOffAllLeds();
int ledSetUp();

#endif //__FUNCTIONS__
