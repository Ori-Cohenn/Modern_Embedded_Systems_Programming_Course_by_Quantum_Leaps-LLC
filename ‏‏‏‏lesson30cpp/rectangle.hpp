#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "shape.hpp"

/* Rectangle's attributes... */
class Rectangle : public Shape{
     /* inherited Shape, has to be written first!*/
private:
    /* attributes specific to this class */
    uint16_t width;
    uint16_t height;

public: /* Rectangle's operations... */
	Rectangle(int16_t x0, int16_t y0,
			  uint16_t w0, uint16_t h0);
	void draw() const ;
	uint32_t area() const;	
};



#endif /* RECTANGLE_H */

