
// ifndef -> if not define
#ifndef __HEADER_H__ //protiction against multiple inclusion running preproccesor
#define __HEADER_H__

int delayfunc(int volatile delay);
int turnOffAllLeds();
int ledSetUp();
int *swap(int *x, int *y);
unsigned fact(unsigned n);

#endif // __HEADER_H__