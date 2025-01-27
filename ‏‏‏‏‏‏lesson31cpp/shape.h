#ifndef SHAPE_H_
#define SHAPE_H_

#include <cstdint>  // standard integer types

class Shape { //change typedef with class an includes it's methods
protected:     /* Shape's attributes... Inherited attributes are marked as protected to allow access in derived classes.*/
    std::int16_t x; // x-coordinate of Shape's position // allowing negetive numbers for shapes off screen compare to unsigned numbers i used so far
    std::int16_t y; // y-coordinate of Shape's position

public:    /* Shape's operations...... class operations need to be publicly accessible*/
    Shape(std::int16_t x0, std::int16_t y0); //countractor (__init__ in python) taking "this" by defualt as class acts
    void moveBy(std::int16_t dx, std::int16_t dy);
    uint16_t distanceFrom(Shape const * other) const; // distance between shapes

	virtual void draw() const ;
	virtual uint32_t area() const;	
};

void drawGraph(Shape const *graph[]);

#endif // SHAPE_H_