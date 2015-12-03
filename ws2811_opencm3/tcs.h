
//http://www.elecfreaks.com/wiki/index.php?title=Programmable_Color_Light-to-Frequency_Converter_Module

#include<stdlib.h>

#ifndef _TCS_H_
#define _TCS_H_


void tcsInit(void);

void tcsOn(void);

void tcsOff(void);

void tcsGetRGB(uint8_t *r, uint8_t *g, uint8_t *b);

#endif
