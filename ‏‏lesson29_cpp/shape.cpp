#include "shape.h" /* Shape *class* interface */

Shape::Shape(int16_t x0, int16_t y0)
:x(x0),
 y(y0)//constructor initializer list
 {
 }

void Shape::moveBy(int16_t dx, int16_t dy) { //:: is scope resolution op is for telling the comipler its from a class
    this->x += dx; //this is like "self" in Python, and *me in C
    this->y += dy;
}

uint16_t Shape::distanceFrom(Shape const * other) const
{
    int16_t dx = x - other->x; /* x - C++ compiler will recognize the names of class attributes and will implicitly add the “this” pointer in front of them.*/
    int16_t dy = y - other->y;
    if (dx < 0) {
       dx = -dx;
    }
    if (dy < 0) {
       dy = -dy;
    }
    return dx + dy;
	/*This function uses the simple “taxicab” geometry, which is adequate 
	for Shapes but does not require extracting square root as the 
	standard Cartesian metric.*/
}
