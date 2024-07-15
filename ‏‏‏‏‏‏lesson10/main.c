/*
	this lesson is about pitfalls to avoid when using functions
	such as	stack-overflow, the stm32f407 startup config set the stack size to be 1KB (Stack_Size      EQU     0x00000400)
	so in this example if i run the fact() requresrvely 3 times i get only from foo[] a 1200 KB of data (400*3, unsigned int is 4 Bytes * 100)
	can change the heap and stack size in the startup_stm32....s 
	the second part of the lesson focused on pointers.
	all local
	why returning a pointer to a local variable is always a
	bad idea, because such pointers will always fall above the stack after
	the function returns. The more technical term is that all local
	variables go out of scope when the function returns, so they no longer
	even exist and can't be accessed.
	The remedy for this problem is quite simple, actually. Instead of using
	local variables on the stack, use local variables that are not on the stack.

	In C the static keyword used in front of a local variable tells the
	compiler to allocate the variable outside of the stack, so that it
	outlives any call to the function and therefore can be accessed even
	after the function returns.
*/

#include "stm32f4xx.h"
#include "delay.h"

#define myGPIOD_base	 0x40020C00				
#define LED_GREEN	 		(1UL<<12)
#define LED_GREEN_OFF	(1U<<(12+16))
#define LED_ORANGE 		(1U<<13)
#define LED_RED		 		(1U<<14)
#define LED_RED_OFF		(1U<<(14+16))
#define LED_BLUE	 		(1U<<15)

int turnOffAllLeds();
int ledSetUp();
int *swap(int *x, int *y);
unsigned fact(unsigned n);

int turnOffAllLeds(){
	GPIOD->BSRR =  1<<(12+16); // Set the BSRR bit 12 + 16->F to 1 to turn respective LED off
	GPIOD->BSRR =  1<<(13+16); // Set the BSRR bit 13 + 16 to 1 to turn respective LED off
	GPIOD->BSRR =  1<<(14+16); // Set the BSRR bit 14 + 16 to 1 to turn respective LED off
	GPIOD->BSRR =  1<<(15+16); // Set the BSRR bit 15 + 16 to 1 to turn respective LED off
	return 0;
}


int ledSetUp(){
	// Configue LEDs
	//RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // Enable the clock of port D of the GPIO-> 0x1UL << RCC_AHB1ENR_GPIODEN_Pos=3u->0x0...08
	RCC->AHB1ENR |=	(1UL<<3);
	GPIOD->MODER |= GPIO_MODER_MODER12_0; // Green LED, set pin 12 as output
	GPIOD->MODER |= GPIO_MODER_MODER13_0; // Orange LED, set pin 13 as output
	GPIOD->MODER |= GPIO_MODER_MODER14_0; // Red LED, set pin 14 as output
	GPIOD->MODER |= GPIO_MODER_MODER15_0; // Blue LED, set pin 15 as output
	// Turn off LEDs
	turnOffAllLeds();
	return 0;
}

unsigned fact(unsigned n){
	// 0!-> 1 , 1!=1, 2!=2*1, n!=n*(n-1)! for n>0
	unsigned foo[6];
	foo[n]=n;
	if(n==0U){
		return 1U;
	}
	else{
	//return n*fact(n-1U);
	return foo[n]; // index out of bound making the stack to overflow
	}
	return 0U;
}

int *swap(int *x, int *y){
	static int tmp[2]; // no longer part of stack but sitting on the regular RAM memory.
	tmp[0]= *x;
	tmp[1]= *y;
	*x=tmp[1];
	*y=tmp[0];
	return tmp;
}

int main(){
	/*unsigned volatile x;
	//x=fact(0U);
	//x=2U+3U*fact(1U);
	(void)fact(7U);	*/
	int x=1<<21;
	int y=x/2;
	ledSetUp();
	*((unsigned volatile long *)(myGPIOD_base+0x18)) = LED_GREEN;
	while(1){
		int *p=swap(&x,&y);
		GPIOD->BSRR = LED_RED; // 0x18 is BSSR reg for  bit set/reset register!
		delayfunc(p[0]);
		GPIOD->BSRR  = LED_RED_OFF;
		delayfunc(p[1]);
	}
	return 0;
}

