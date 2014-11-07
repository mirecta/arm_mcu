/*
*
* Copyright (C) Miroslav Talasek <miroslav.talasek@seznam.cz>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
*/
#include <stm32f4xx.h>
#include "stm32f4_gpio.h"
#include "stm32f4_util.h"
#include "glibm.h"
#include "flame.h"
#include <stdio.h>

#define LED_PIN 5
#define LED_ON() GPIOA->BSRRL |= (1 << 5)
#define LED_OFF() GPIOA->BSRRH |= (1 << 5)

#define RGB(r,g,b) (((b >> 3) << 11) | ((g >> 2) << 5) | ((r>> 3)))

#ifdef __cplusplus
 extern "C" {
#endif


#ifdef __cplusplus
 }
#endif




int main() {
    char a = 1;
    char shbuf[128];

    float x = 12.5;
    float b = 1.8;
    float c;


    c = x*b;

    initDelay();
    GlibM gl(160,128);
    Flame fl;

    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    GPIOB->MODER |= (GP_MODE_OUTPUT << (GP_PIN9 << 1));
    GPIOB->OSPEEDR |= (GP_SPEED_HIGH << (GP_PIN9 << 1));
    GPIOB->BSRRH = 1 << GP_PIN9;

    
    gl.clearScr();
   // gl.fillRoundRect(30,30,50,70,10,RGB(0,255,0));
    gl.setTextSize(1);
    gl.write('A');
     //snprintf(buf,128,"milis:  ");
     //gl.drawTextXY(0,0,buf);

    gl.printfXY(0,0,"Hello %d",a);


    //gl.clearScr(RGB(0,255,0));
    //    gl.drawLine(5,5,20,20,RGB(255,0,255));
    //GPIOB->BSRRL = 1 << GP_PIN9;

    while(1){
         
        fl.compute(220,(a%2 == 0));
        gl.rectToFill(0,159,47,127);
        for(int i = 0; i < FLAME_WIDTH * FLAME_HEIGHT; ++i){
            gl.rectFill(fl.getColor(fl.data()[i]));
        }
        ++a;
      Delay(20);

    }
    
  
//------------------------------------------------------------------------------

}
