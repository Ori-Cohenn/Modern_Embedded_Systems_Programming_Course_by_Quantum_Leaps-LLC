/* This lesson is about Polymorphism in C++!*/

#include "shape.h"
#include "rectangle.h"
#include "circle.h" 

#include "qassert.h" // for Q_ASSERT

Q_DEFINE_THIS_FILE; // file name for assertions

Shape s1(1, 2); // static allocation
Rectangle r1(1, 2, 15, 10); // static allocation of Rectangle
uint32_t a;

Circle c1(3, 4, 10);

int main(void) {
    Shape s2(3, 4); // automatic allocation
    Shape *ps3 = new Shape(5, 6); // dynamic allocation
    //Shape const *ps1 = &s1; // const pointer to s1
	
	Shape *ps = &r1; /*Upcasting! */
	
	r1.draw(); /* early biding*/
	ps->draw(); /* late biding*/
	a = ps->area();
	
	Shape const *graph[]{
			&c1,
			&r1,
			ps3,
			(Shape *)0
	};
	drawGraph(graph);
	
    s1.moveBy(7, 8);
    s2.moveBy(9, 10);
    ps3->moveBy(-1, -2);
    //ps1->moveBy(-3, -4);

    r1.draw();
    a = r1.area();
    r1.moveBy(7, 8);
    Q_ASSERT(r1.distanceFrom(&r1) == 0U);

    Q_ASSERT(s1.distanceFrom(&s1) == 0U);
    Q_ASSERT(s1.distanceFrom(&s2) ==
             s2.distanceFrom(&s1));
    Q_ASSERT(s1.distanceFrom(&s2) <=
             s1.distanceFrom(ps3)
           + ps3->distanceFrom(&s2));

    delete ps3;

    while (1) {
    }

    //return 0; // unreachable code
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
summary of this class:
Polymorphism is a uniquely object-oriented concept.
It has no direct analog in procedural languages like C.
The lesson starts by copying the previous lesson 30 directory.
The new directory is renamed to lesson 31.
The Rectangle class inherited attributes and operations from the Shape class.
Rectangle added its own operations, like draw() and area().
These operations make sense for the Shape class too.
The Shape class provides an interface for draw() and area().
The implementation is generic, as it’s unclear how to implement it.
Testing involves upcasting a Rectangle pointer to Shape.
When calling Shape operations, the Shape implementation is executed.
Adding the virtual keyword to Shape operations changes this behavior.
With virtual, the Rectangle implementation is executed.
This is polymorphism.
Polymorphism allows operations to take multiple forms.
It enables writing higher-level, generic code.
An example is the drawGraph() function for multiple Shape objects.
Polymorphism ensures the correct draw() implementation is used.
The compiler warns when methods implicitly inherit virtual.
Declaring them explicitly virtual resolves the warning.
Polymorphism enables extensibility in design.
The Circle class is introduced as a new subclass of Shape.
The Circle class overrides draw() and area() operations.
Circle is tested as part of the graph array.
The graph adapts to include Circle without modifying drawGraph().
Polymorphism uses a VTABLE in ROM and VPTR in RAM.
The VPTR points to the VTABLE containing virtual function addresses.
Regular function calls use early binding.
Virtual calls use late binding or dynamic binding.
Late binding adds overhead but offers flexibility.
The compiler initializes VPTR in constructors.
VPTR points to the correct class’s VTABLE at runtime.
Polymorphism provides extensibility with manageable overhead.
The next lesson will cover implementing late binding in C.
*/