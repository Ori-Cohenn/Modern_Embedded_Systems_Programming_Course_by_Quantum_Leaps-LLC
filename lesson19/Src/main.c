/*
This lesson is about switching to gnu-arm compiler and ellipse based IDE that is supporting the stm32f407VGT6-> STM32CubeIDE!
also rewriting the entire code to be cleaner and more closed to the accual lesson i following.
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
