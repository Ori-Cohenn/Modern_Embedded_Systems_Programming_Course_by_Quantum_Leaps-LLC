#ifndef CIRCLE_H_
#define CIRCLE_H_

#include "shape.h"

/* Circle's attributes... */
class Circle : public Shape{
     /* inherited Shape, has to be written first!*/
private:
    /* attributes specific to this class */
    uint16_t radius;

public: /* Circle's operations... */
	Circle(int16_t x0, int16_t y0,
			  uint16_t r0);
	virtual void draw() const;
	virtual uint32_t area() const;	
};



#endif /* CIRCLE_H_ */

