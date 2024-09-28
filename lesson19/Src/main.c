/*
This lesson is about interrupts.
*/

#include "bsp.h"

int main(){
    BSP_init();
    BSP_ledGreenOn();
    BSP_ledOrangeOn();
	while(1){
		__asm("NOP");
	}
	return 0;
}
