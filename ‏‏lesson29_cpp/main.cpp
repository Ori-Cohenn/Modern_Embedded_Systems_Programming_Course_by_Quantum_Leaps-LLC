/*
This lesson is about introduction to object-oriented programming (OOP) in low-level inside embedded microcontroller.
 */


#include "shape.h"
#include "qassert.h" // for Q_ASSERT


Q_DEFINE_THIS_FILE



Shape s1(1,2); /*static allocation (objects- instance of class)*/

int main(void)
{
	Shape s2(3,4); /*automatic allocation (objects- instance of class)*/
	Shape *ps3 = new Shape(5,6); /*A dynamic object, like ps3 here, is allocated in C++ by means of the 
								C++ "new" operator, which is followed by the class constructor.*/
/*	Shape const *ps1 = &s1; //const pointer to s1 check that the moveBy() operation is not 
							allowed for const Shape objects.*/
	
	
	
	s1.moveBy(7, 8); //. - accessing to function in class
	s2.moveBy(9, 10);
	ps3->moveBy(-1, -2);
//  Shape_moveBy(ps1, -3, -4);

	
	/*
	- the distance from a shape to the same shape must be zero
	- the distance from shape1 to shape2 must be the same as shape2 to 
	shape1
	- and the distance between two shapes s1 and s2 must be less or equal
	to the sum of distances to any other shape, like ps3.*/
	
    Q_ASSERT(s1.distanceFrom(&s1) == 0U);
    Q_ASSERT(s1.distanceFrom(&s2) ==
             s2.distanceFrom(&s1));
    Q_ASSERT(s1.distanceFrom(&s2) <=
             s1.distanceFrom(ps3)
           + ps3->distanceFrom(&s2));
	
	delete ps3; //delete = free
	
}
//............................................................................
extern "C" {
void Q_onAssert(char const *module, int loc) {
    // TBD: damage control
    (void)module; // avoid the "unused parameter" compiler warning
    (void)loc;    // avoid the "unused parameter" compiler warning
    while (1) {   // hang in an endless loop
    }
}

}  // extern "C"

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