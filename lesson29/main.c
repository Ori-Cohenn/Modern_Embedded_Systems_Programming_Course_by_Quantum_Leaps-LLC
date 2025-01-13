/*
This lesson is about introduction to object-oriented programming (OOP) in low-level inside embedded microcontroller.
 */

#include "qpc.h"
#include "bsp.h"
#include "shape.h"
#include <stdlib.h> /* for malloc() and free() */


Q_DEFINE_THIS_FILE

QXSemaphore SW1_sema;


uint32_t stack_blinky1[40];
QXThread blinky1;
void main_blinky1(QXThread * const me)
{
	while (1){
        BSP_sendMorseCode(0xA8EEE2A0U); /* "SOS" the function shared between blinky1&3 */
        QXThread_delay(1U); /* block for 1 tick */
	}
}

uint32_t stack_blinky2[40];
QXThread blinky2;
void main_blinky2(QXThread * const me)
{
	while (1){
		uint32_t volatile i;

		QXSemaphore_wait(&SW1_sema,  /* pointer to semaphore to wait on */
						QXTHREAD_NO_TIMEOUT); /* timeout for waiting no timeout means forever */

		for (i = 3*1500U; i != 0U; --i) {
			//BSP_ledOrangeOn();
			//BSP_ledOrangeOff();
			BSP_ledOrangeToggle();
		}
	}
}
uint32_t stack_blinky3[40];
QXThread blinky3;
void main_blinky3(QXThread * const me)
{
  while (1)
  {
	BSP_sendMorseCode(0xE22A3800U); /* "TEST" */
	BSP_sendMorseCode(0xE22A3800U); /* "TEST" */
	QXThread_delay(5U);
  }
}


Shape s1; /*static allocation (objects- instance of class)*/

int main(void)
{
	Shape s2; /*automatic allocation (objects- instance of class)*/
	Shape *ps3 = malloc(sizeof(Shape));
/*	Shape const *ps1 = &s1; //const pointer to s1 check that the moveBy() operation is not 
allowed for const Shape objects.*/
	
	Shape_ctor(&s1,1,2); /* init by constructor also used at QXThread_ctor*/
	Shape_ctor(&s2,3,4);
	Shape_ctor(ps3,5,6);
	
	Shape_moveBy(&s1, 7, 8);
    Shape_moveBy(&s2, 9, 10);
    Shape_moveBy(ps3, -1, -2);
//  Shape_moveBy(ps1, -3, -4);

	
	/*
	- the distance from a shape to the same shape must be zero
	- the distance from shape1 to shape2 must be the same as shape2 to 
	shape1
	- and the distance between two shapes s1 and s2 must be less or equal
	to the sum of distances to any other shape, like ps3.*/
	
    Q_ASSERT(Shape_distanceFrom(&s1, &s1) == 0U); /* distance of a shape to itself must be 0*/
    Q_ASSERT(Shape_distanceFrom(&s1, &s2) ==
             Shape_distanceFrom(&s2, &s1));
    Q_ASSERT(Shape_distanceFrom(&s1, &s2) <=
             Shape_distanceFrom(&s1, ps3)
           + Shape_distanceFrom(ps3, &s2));
	
	free(ps3);
	
	QF_init();
	BSP_init();
    /* initialize the SW1_sema semaphore as binary, signaling semaphore */
    QXSemaphore_init(&SW1_sema, /* pointer to semaphore to initialize */
                     0U,  /* initial semaphore count (singaling semaphore) */
                     1U); /* maximum semaphore count (binary semaphore) */

    /* initialize and start blinky1 thread */
    QXThread_ctor(&blinky1, &main_blinky1, 0);
    QXTHREAD_START(&blinky1,
                   5U, /* priority */
                   (void *)0, 0, /* message queue (not used) */
                   stack_blinky1, sizeof(stack_blinky1), /* stack */
                   (void *)0); /* extra parameter (not used) */

    /* initialize and start blinky2 thread */
    QXThread_ctor(&blinky2, &main_blinky2, 0);
    QXTHREAD_START(&blinky2,
                   2U, /* priority */
                   (void *)0, 0, /* message queue (not used) */
                   stack_blinky2, sizeof(stack_blinky2), /* stack */
                   (void *)0); /* extra parameter (not used) */

    /* initialize and start blinky3 thread */
    QXThread_ctor(&blinky3, &main_blinky3, 0);
    QXTHREAD_START(&blinky3,
                   1U, /* priority */
                   (void *)0, 0, /* message queue (not used) */
                   stack_blinky3, sizeof(stack_blinky3), /* stack */
                   (void *)0); /* extra parameter (not used) */

				   
    /* transfer control to the RTOS to run the threads */
    return QF_run();
}

/*
Previous lessons covered RTOS and shared-state concurrency programming paradigm.
The most complex parts of RTOS weren't context switching, but rather blocking and synchronization mechanisms.
The BSP (Board Support Package) already demonstrated some OOP concepts through abstraction and information hiding.
The Shape interface is introduced to demonstrate better OOP practices than the fixed BSP implementation.
Each Shape has x and y coordinates using 16-bit signed integers to allow negative values for off-screen positions.
The Shape "constructor" function takes a const pointer called "me" as its first parameter.
The "me" pointer corresponds to "this" in C++ and "self" in Python.
Function names must start with the structure name by convention in C implementation.
The distance calculation uses "taxicab" geometry to avoid square root calculations.
Three ways to create Shape objects are demonstrated: static allocation, automatic stack allocation, and dynamic heap allocation.
Dynamic allocation requires setting heap size to 1KB (1024 bytes) in the project options.
The C++ implementation requires stricter type checking for void pointers.
C++ classes use the 'private' keyword for attributes and 'public' for operations.
The C++ constructor must have the same name as the class and cannot return a value.
Static objects in C++ have their constructors called before the main() function.
The C++ 'new' operator internally calls malloc() with the same 4-byte size as the C implementation.
C++ scope resolution operator (::) replaces underscores used in C implementation.
Constructor initializer lists are the preferred C++ way to initialize class attributes.
The dot operator (.) is used for object operations in C++, while arrow operator (->) is used for pointer operations.
Race conditions can occur even with encapsulated objects when shared between RTOS threads.
The example shows a race condition between blinky1 modifying s1 and blinky2 using s1.
Pressing SW1 button on the TivaC LaunchPad board triggers assertion checks.
The Shape class implementation is identical in machine code between C and C++.
The debugger shows the "this" pointer in C++ behaving exactly like the "me" pointer in C.
All class operations in both implementations follow the ARM Application Procedure Call Standard.
Active objects are introduced as a solution to concurrent programming challenges.
Project files are available at state-machine.com/quickstart.
The lesson sets up for future topics on inheritance and polymorphism.
The Q_onAssert() handler is used to catch assertion failures during debugging.
*/