/*
This lesson is Polymorphism in C.
0x0800 0000 - 0x080F FFFF flash memory, used when using const keyword
*/

#include "qpc.h"
#include "bsp.h"
#include "shape.h"
#include "rectangle.h"
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
Rectangle r1; /* static allocation */
uint32_t a;

int main(void)
{
	Shape s2; /*automatic allocation (objects- instance of class)*/
	Shape *ps3 = malloc(sizeof(Rectangle));
/*	Shape const *ps1 = &s1; //const pointer to s1 check that the moveBy() operation is not 
allowed for const Shape objects.*/
	
	
	Shape_ctor(&s1, 1, 2);
    Shape_ctor(&s2, 3, 4);
    Rectangle_ctor((Rectangle *)ps3, 5, 6, 10, 13);
    Rectangle_ctor(&r1, 1, 2, 15, 10);

    Shape const *graph[] = {
        &s1,
        (Shape *)&r1,
        ps3,
        (Shape *)0
    };
    drawGraph(graph); // <== generic drawGraph() operation	
	
	Shape_moveBy(&s1, 7, 8);
    Shape_moveBy(&s2, 9, 10);
    Shape_moveBy(ps3, -1, -2);
	//Shape_moveBy(ps1, -3, -4);
	

	Rectangle_draw(&r1);
    a = Rectangle_area(&r1);
    Shape_moveBy((Shape *)&r1, 7, 8);
    Q_ASSERT(Shape_distanceFrom((Shape *)&r1, (Shape *)&r1) == 0U);
	
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
Polymorphism allows different methods for the same inherited operation.
C++ implements polymorphism using virtual functions.
The late-binding mechanism selects the function based on the object type at runtime.

Unlike encapsulation and inheritance, polymorphism is not free in C.
It adds complexity and overhead.
If extensive polymorphism is needed, switching to C++ is better.
However, polymorphism in C can be useful in libraries.
It keeps complexities hidden from the application developers.

To implement polymorphism in C, you need a VPTR and a VTABLE.
The VPTR is the first attribute in the Shape class.
It is a pointer to a constant VTABLE structure stored in ROM.

A VTABLE is a struct of function pointers.
Each function pointer stores the address of a virtual function.
The function signature determines the type of the function pointer.
To declare a function pointer, parentheses are used to separate the asterisk from the return type.

Virtual function calls need an extra step.
Instead of calling functions directly, the VPTR is dereferenced.
Then the VTABLE is accessed to get the function pointer.
Finally, the function is called with the "me" pointer and other arguments.

One way to implement virtual function calls is using wrapper functions.
For example, Shape_draw_vcall() calls the draw function via the VPTR.
Another option is using inline functions.
This avoids the overhead of extra function calls.
C99 introduced *inline* functions, but older C89 compilers do not support them.

Another alternative is macros.
Macros provide only textual substitution.
They lack type safety and can cause issues.
Using inline functions is preferred over macros.

Next, the VTABLE needs to be defined in ROM.
A constant instance of the ShapeVtable struct is created.
It is initialized with the function addresses for draw() and area().
The VPTR in each Shape object must be set to this VTABLE.

The VPTR is initialized in the constructor.
The constructor of the base class sets the VPTR to the base class VTABLE.
Subclasses override this VPTR with their own VTABLEs.

In the Rectangle class, a separate VTABLE is defined.
The draw() and area() functions in Rectangle must match the base class signatures.
Function pointer casting is needed to fit the ShapeVtable structure.

The drawGraph() function demonstrates polymorphism in action.
It loops over an array of Shape pointers.
Each call to draw() dynamically selects the correct method.

In debugging, the VPTR setup works as expected.
The Shape constructor initializes the VPTR to the Shape VTABLE.
The Rectangle constructor overrides it with the Rectangle VTABLE.
Late-binding machine instructions in C match those generated by C++.

Another way to implement virtual functions in C is embedding the VTABLE in each object.
This eliminates the VPTR level of indirection.
However, it increases RAM usage since each object stores a copy of the VTABLE.
This method is described in “Design Patterns for Embedded Systems in C” by Bruce Powel Douglass.

Polymorphism is useful when different behaviors are selected at runtime.
A bad practice is using polymorphism for product versioning.
Instead, different Board Support Packages (BSPs) should be created for different products.
The book “Large Scale C++ Software Design” by John Lakos discusses physical design techniques.

This concludes the lessons on object-oriented programming in embedded systems.
The next lesson will cover event-driven programming.

Based on the text, here are the guidelines for when to use and not use polymorphism:
WHEN TO USE POLYMORPHISM:

When object-specific behavior needs to be selected at runtime
When you see or anticipate switch/if-then-else statements scattered throughout your project that select behavior based on object type
When you need automatically extensible code that allows adding/removing different subclasses without changing existing code

WHEN NOT TO USE POLYMORPHISM:

When the selection based on object type does not need to happen at runtime
For managing product lines of related but slightly different products where:

The code ends up in only one specific product
Selection can happen at compile-time and link-time instead
Different implementations can be provided through BSP (Board Support Package) interface



The author suggests that for product line management, instead of using polymorphism:

Design a clean BSP interface
Provide different implementations for different products
Use physical design (proper code partitioning into directories and files)
Combine modules at link-time rather than using runtime techniques

The author emphasizes that the only valid reason for using polymorphism is when object-specific behavior must be selected at runtime.
 Otherwise, other design approaches may be more appropriate.
*/