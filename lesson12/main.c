/*
	this lesson is about Structures in C and Cortex Microcontroller Software Interface Standard (CMSIS).
	M4 can run M0\1\3 instraction if we change it is the options of the compiler.
	alignment of the data in memory
	matters to the processor. As an embedded programmer you absolutely need
	to know about it, because otherwise you will never understand why the
	compiler inserts padding into your structures. The compiler prefers to
	keep the data aligned instead of wasting the CPU cycles to access the
	mis-aligned data.
	you need
	to use __packed structures judiciously only when you absolutely need to
	avoid padding.
*/

#include <stdint.h> 
#include "stm32f4xx.h"
#include "header.h"

#define myGPIOD_base	 0x40020C00				
#define LED_GREEN	 		(1UL<<12)
#define LED_GREEN_OFF	(1U<<(12+16))
#define LED_ORANGE 		(1U<<13)
#define LED_RED		 		(1U<<14)
#define LED_RED_OFF		(1U<<(14+16))
#define LED_BLUE	 		(1U<<15)

// struct for lcd display with 2 members x and y using typedef to short the next sturct decleration for variables.

typedef struct{ //no tag for this stuct, should be in linked list, trees and so on...
	uint8_t y;
	uint16_t x;
} Point; 

Point p1,p2;

/* in sturct the order of members matters! if we change the order of them
then the order of them in the memory will also changed!
p1.x as any other half-word variable and uses the already familiar STRH
instruction to store a value in it.
But the compiler evidently thinks that the size of the Point struct is 4
bytes, not 3. *(8+16 bits are 3 bytes, waste of 1 byte by paddiny by the compiler)*
In the evaluation of the expression, you can see again that p1.y is
treated as a byte-size variable, because the result of the expression is
stored with the STRB instruction.
there is a way to save that byte in embedded compilers like the one im using , keil compiler, __packed. 
*/

//typedef struct{
//	uint8_t y;
//	uint16_t x;
//}__packed Point; 

// a stuct of a sturct
typedef struct{
	Point top_left;
	Point bottom_right;
} Window;

typedef struct{
	Point corners[3];
} Triangle;

int main(){
	Point p1,p2;
	p1.x=sizeof(Point);
	p1.y=0xAAU;
	// variables of structure types are called "instances"
	Window w,w2;
	Triangle t;
	
	Point *pp=&p1;
	Window *wp=&w2;;
	
	
	w.top_left.x=1U;
	w.bottom_right.y=2U;
	
	t.corners[0].x=1U;
	t.corners[2].y=2U;
	
	p2=p1;
	w2=w;
	
	(*pp).x=1U;
	
	(*wp).top_left=*pp;
	
	pp->x=1U;
	
	wp->top_left=*pp;

	ledSetUp();
	*((unsigned volatile long *)(myGPIOD_base+0x18)) = LED_GREEN;
	while(1){
		GPIOD->BSRR = LED_RED; // 0x18 is BSSR reg for  bit set/reset register!
		delayfunc(1<<21);
		GPIOD->BSRR  = LED_RED_OFF;
		delayfunc(1<<21);
	}
	return 0;
}

