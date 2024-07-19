/*
	this lesson is about data type and stdint.h.
	*'int' or 'unsigned' can be 32-bit wide on a 32-bit machine,
	such as ARM, but they can be only 16-bit wide on a 16-bit or 8-bit
	machines, such as MSP430 or AVR.
	*the compiler can and will change the order of your definitions, 
	so you should never assume that the order will be preserved.
	*ARM processor has special instructions LDRB and STRB to read and write bytes, LDRH and
	STRH to read and write half-words, and plain LDR and STR to read and
	write whole words into memory.
	* small integers in computation are converted to the platform's "int" instead of the left-hand side's datatype,
	and a signed integer used with unsigned ones is converted to unsigned int.
	Implicit Type Conversion: When an operation involves both signed and unsigned integers,
	the signed integer is converted to an unsigned integer.
	Comparison Rules: When comparing signed and unsigned integers,
	the signed integer is converted to an unsigned integer, which can lead to unexpected results if not handled carefully.
	Promotion to a Common Type: For binary operations (like comparisons), 
	both operands are promoted to a common type before the operation is performed. 
	The promotion follows the usual arithmetic conversions,
	which often mean converting both operands to the type of the unsigned operand if they have different signedness.

*/

#include <stdint.h> // made up in c99 standard
#include "stm32f4xx.h"
#include "delay.h"

#define myGPIOD_base	 0x40020C00				
#define LED_GREEN	 		(1UL<<12)
#define LED_GREEN_OFF	(1U<<(12+16))
#define LED_ORANGE 		(1U<<13)
#define LED_RED		 		(1U<<14)
#define LED_RED_OFF		(1U<<(14+16))
#define LED_BLUE	 		(1U<<15)

uint8_t  u8a, u8b;
uint16_t u16c, u16d;
uint32_t u32e, u32f;

int8_t  s8;
int16_t s16;
int32_t s32;


int main(){
	ledSetUp();
	
	u8a  = sizeof(u8a);
	u16c = sizeof(uint16_t);
	u32e = sizeof(uint32_t);

	u8a  = 0xa1U;
	u16c = 0xc1c2U;
	u32e = 0xe1e2e3e4U;

	u8b  = u8a;
	u16d = u16c;
	u32f = u32e;

	u16c = 40000U;
	u16d = 30000U;
	//u32e = u16c + u16d; // NOT portable! meaning not working on 16bit machine
	u32e = (uint32_t)u16c + u16d;

	u16c = 100U;
	//s32  = 10 - u16c;  // NOT portable! must not mixed signed and unsigned
	//s32  = 10 - (int16_t)u16c; // INCORRECT: unintended sign extension becuase 10 is int32bit
	s32  = 10 - (int32_t)u16c;

	//if (u32e > -1) {  // ALWAYS false!, becuse of the mixed signed, -1 will be 0xFFFFFFFF= max unisgned int 32 bit
	if ((int32_t)u32e > -1) {
			u8a = 1U;
	}
	else {
			u8a = 0U;
	}

	u8a = 0xffU;
	 /* if (~u8a == 0x00U) {
	ALWAYS false! because the in binary ops compiler promoted to a common type before the operation is performed 
	so the NOT op will not include the MSB
	~ff=~(1111 1111)=1000 0000+1 = -1 in decimal
	and u8a=-1 (2's compliment)
	the fix for it is checking the whole byte by casting u8a back to uint88_t
	*/
	if ((uint8_t)(~u8a) == 0x00U) { // common when  byte represents a checksum over some data and you want to make sure that the checksum adds up!
			u8b = 1U;
	}


	*((unsigned volatile long *)(myGPIOD_base+0x18)) = LED_GREEN;
	while(1){
		GPIOD->BSRR = LED_RED; // 0x18 is BSSR reg for  bit set/reset register!
		delayfunc(1<<21);
		GPIOD->BSRR  = LED_RED_OFF;
		delayfunc(1<<21);
	}
	return 0;
}

