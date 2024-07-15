#include "delay.h"

int delayfunc(int delay){
	for(volatile int i=0;i<delay;i++) __asm("NOP");
	return 0;
}