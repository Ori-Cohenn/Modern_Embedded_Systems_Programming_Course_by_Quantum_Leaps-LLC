#ifndef SHAPE_HPP_
#define SHAPE_HPP_

#include <cstdint>  // standard integer types

class Shape { //change typedef with class an includes it's methods
private:     /* Shape's attributes... inaccessible outside of the class*/ 
    std::int16_t x; // x-coordinate of Shape's position // allowing negetive numbers for shapes off screen compare to unsigned numbers i used so far
    std::int16_t y; // y-coordinate of Shape's position

public:    /* Shape's operations...... class operations need to be publicly accessible*/
    Shape(std::int16_t x0, std::int16_t y0); //countractor (__init__ in python) taking "this" by defualt as class acts
    void moveBy(std::int16_t dx, std::int16_t dy);
    uint16_t distanceFrom(Shape const * other) const; // distance between shapes
};

#endif // SHAPE_HPP_