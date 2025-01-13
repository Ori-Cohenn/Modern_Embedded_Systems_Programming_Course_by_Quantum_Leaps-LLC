/*
This lesson is about introduction to object-oriented programming (OOP) in low-level inside embedded microcontroller.
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
	Shape *ps3 = malloc(sizeof(Shape));
/*	Shape const *ps1 = &s1; //const pointer to s1 check that the moveBy() operation is not 
allowed for const Shape objects.*/
	
	
	Shape_ctor(&s1, 1, 2);
    Shape_ctor(&s2, 3, 4);
    Shape_ctor(ps3, 5, 6);
    Rectangle_ctor(&r1, 1, 2, 15, 10);

	
	Shape_moveBy(&s1, 7, 8);
    Shape_moveBy(&s2, 9, 10);
    Shape_moveBy(ps3, -1, -2);
	//Shape_moveBy(ps1, -3, -4);
	
	/*
	- the distance from a shape to the same shape must be zero
	- the distance from shape1 to shape2 must be the same as shape2 to 
	shape1
	- and the distance between two shapes s1 and s2 must be less or equal
	to the sum of distances to any other shape, like ps3.*/
	
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
Today’s topic is class inheritance.  
Inheritance allows creating new classes based on existing ones.  
It reuses attributes and operations.  
In the previous lesson, you learned about encapsulation and the Shape class.  
Encapsulation packages data and functions into classes.  
The Shape class represented shapes on an LCD display.  
Its operations were applied through a defined class interface.  

To create a Rectangle class, derive it from the Shape class.  
The Shape instance is added as the first attribute, named "super."  
Additional attributes like width and height are included.  
Operations such as draw() and area() are defined.  
In the Rectangle constructor, call the Shape constructor first.  
This ensures inherited attributes are initialized.  
The memory layout places Shape attributes at the beginning.  
This allows a Rectangle pointer to be safely cast to a Shape pointer.  
Shape operations like moveBy() can also be applied to Rectangle.  
This process is known as upcasting.  
It establishes an "is-a" relationship, meaning "Rectangle is a Shape."  

The concept of inheritance is best understood through a biological classification analogy.  
For example, "a Rectangle is a Shape" makes logical sense.  
Inheritance allows reusable and extensible relationships.  
In contrast, composition uses a "has-a" relationship.  
Composition includes components but does not support upcasting.  

In C++, replace typedef struct with the class keyword.  
Use public Shape to indicate inheritance.  
The constructor of the derived class calls the base class constructor.  
This is done using a constructor initializer list.  
The explicit "me" pointer is replaced with the implicit "this" pointer.  
Inherited attributes are marked as protected to allow access in derived classes.  
The C++ implementation follows the same logic as in C.  
C++ syntax enhances clarity and efficiency.  

Static constructors in C++ are invoked before main().  
The memory layout aligns inherited attributes at the beginning of the object.  
Operations inherited from Shape automatically upcast the "this" pointer.  
This ensures compatibility with base class operations.  

Inheritance efficiently reuses attributes and operations.  
The behavior observed in C++ mirrors the emulation implemented in C.  
The next lesson will explore polymorphism.  
Visit the course website for class notes and project files.  
*/