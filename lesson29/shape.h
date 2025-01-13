#ifndef SHAPE_H
#define SHAPE_H

#include <stdint.h>

/* Shape's attributes... */
typedef struct {
    int16_t x; /* x-coordinate of Shape's position */
    int16_t y; /* y-coordinate of Shape's position */
} Shape;
// allowing negetive numbers for shapes off screen compare to unsigned numbers i used so far
/* Shape's operations... */
void Shape_ctor(Shape * const me, int16_t x0, int16_t y0); //constructor, shape * const me->shape instance can change but the pointer cannot
void Shape_moveBy(Shape * const me, int16_t dx, int16_t dy);
uint16_t Shape_distanceFrom(Shape const * const me,
                            Shape const * other); // distance between shapes

/*First, the names of all associated functions start with the name of 
the associated structure.

And second, all these functions take the “me” pointer as the first 
parameter, with the purpose to specify which specific Shape instance 
the function is operating on.

me is allias of "self" in python \ "this" in cpp
*/

#endif /* SHAPE_H */
