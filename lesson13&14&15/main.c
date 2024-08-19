/*
This lesson is about what is a Startup code and editing the generic one.
Startup code is the code that's runs before the main() function.
we can find the report of our build based both the startup code and main() in the map file.

Table 62. Vector table for STM32F405xx/07xx and STM32F415xx/17xx page 375
shows the vector table or in other name, interrupt table.
.o is object we created
arm-none-eabi-objdump -D main.o > main.txt
What is __stackless?
Definition: The __stackless attribute tells the compiler that a function or block of code is stackless, 
meaning it does not use the stack for its execution.
Usage: This attribute is typically used to optimize performance and reduce stack usage, 
which is crucial in embedded systems where memory is limited.
*/


#include "stm32f4xx.h"
#include <stdint.h>
#include "header.h"


int16_t x = -1;
uint32_t y = 4;

int16_t sqr[] = {
    1*1,
    2*2,
    3*3,
    4*4
}; 

typedef struct {
    uint8_t y;
    uint16_t x;
} Point;

Point p1 = {
    123U,
    0x1234U
};
Point p2;

typedef struct {
    Point top_left;
    Point bottom_right;
} Window;

typedef struct {
     Point corners[3];
} Triangle;

Window w = {
    { 123U, 0x1234U },
    { 234U, 0x6789U }
};
Window w2;
Triangle t;

int main(){
  
  Point *pp;
  Window *wp;

  p1.x = sizeof(Point);
  p1.y = 0xAAU;

  w.top_left.x = 1U;
  w.bottom_right.y = 2U;

  t.corners[0].x = 1U;
  t.corners[2].y = 2U;

  p2 = p1;
  w2 = w;

  pp = &p1;
  wp = &w2;

  (*pp).x = 1U;

  (*wp).top_left = *pp;

  pp->x = 1U;
  wp->top_left = *pp;
	ledSetUp();
	*((unsigned volatile long *)(myGPIOD_base+0x18)) = LED_GREEN;
	while(1){
		GPIOD->BSRR = LED_BLUE; // 0x18 is BSSR reg for  bit set/reset register!
		delayfunc(1<<21);
		GPIOD->BSRR = LED_BLUE_OFF;
		delayfunc(1<<21);
	}
	return 0;
}
/*
Summary
Cross-development is a key aspect of embedded systems, where tools like Keil Workbench on a desktop
(host machine) develop programs for a different computer,
such as a LaunchPad board (target machine).
Running the compiler and linker directly on small embedded target machines is impractical.
This contrasts with native development for desktop computers, where development and execution occur on the same machine.

In the embedded build process, source files (e.g., main.c and delay.c) are compiled into object files (main.o and delay.o). 
These object files, along with libraries and a linker script, are then combined by the linker into the final program. 
Object files contain relocatable machine code, which the linker converts into executable code by resolving addresses.

Object files, often found in directories like Debug\Obj, are typically in the ELF (Executable and Linkable Format) format. 
ELF files, containing binary data, can be examined using tools like ielfdumparm.exe from the Keil toolchain or objdump from the GNU compiler collection. 
These tools convert the binary contents into a human-readable text format, facilitating inspection and debugging.
*/
