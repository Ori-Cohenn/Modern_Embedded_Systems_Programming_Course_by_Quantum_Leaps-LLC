
// ifndef -> if not define
#ifndef __DELAY_H__ //protiction against multiple inclusion running preproccesor
#define __DELAY_H__

int delayfunc(int volatile delay);
int turnOffAllLeds();
int ledSetUp();
int *swap(int *x, int *y);
unsigned fact(unsigned n);

#endif // __DELAY_H__