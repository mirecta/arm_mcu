#include <stm32f10x.h>
#include <string.h>
#include <stdlib.h>
#include "stm32f1_util.h"

#include "stm32f10x_conf.h"
#include "gfx.h"



static int uitoa(unsigned int value, char * buf, int max) {
    int n = 0;
    int i = 0;
    unsigned int tmp = 0;

    if (!buf)
        return -3;

    if (2 > max)
        return -4;

    i=1;
    tmp = value;
    if (0 > tmp) {
        tmp *= -1;
        i++;
    }
    for (;;) {
        tmp /= 10;
        if (0 >= tmp)
            break;
        i++;
    }
    if (i >= max) {
        buf[0] = '?';
        buf[1] = 0x0;
        return 2;
    }

    n = i;
    tmp = value;
    if (0 > tmp) {
        tmp *= -1;
    }
    buf[i--] = 0x0;
    for (;;) {
        buf[i--] = (tmp % 10) + '0';
        tmp /= 10;
        if (0 >= tmp) {
            break;
        }
    }
    if (-1 != i) {
        buf[i--] = '-';
    }

    return n;
}

void benchmark(void) {
    uint32_t i, pixels, ms, pps;
    char pps_str[25];
	coord_t height, width, rx, ry, rcx, rcy;
    color_t random_color;
	font_t font;
    gdispClear(Black);
    //gdispSetOrientation(GDISP_ROTATE_90);

	width = gdispGetWidth();
	height = gdispGetHeight();
    font = gdispOpenFont("UI2 Double");

	gdispDrawStringBox(0, 0, width, 30, "GFX - Benchmark", font, White, justifyCenter);

	font = gdispOpenFont("UI2");
	gdispDrawStringBox(0, height/2, width, 30, "500 random rectangles", font, White, justifyCenter);
	
	gfxSleepMilliseconds(3000);
	
	/* seed for the rand() */
	srand(GetMilis());
	pixels = 0;

	//CPU_RESET_CYCLECOUNTER;

	for (i = 0; i < 500; i++) {
		random_color = (rand() % 65535);
		rx = (rand() % (width-10));
		ry = (rand() % (height-10));
		rcx = (rand() % ((width-rx)-10))+10;
		rcy = (rand() % ((height-ry)-10))+10;

		gdispFillArea(rx, ry, rcx, rcy, random_color);
		pixels += (rcx+1)*(rcy+1);
	}

	ms = GetMilis();
	pps = (float)pixels/((float)ms/1000.0f);

	memset (pps_str, 0, sizeof(pps_str));
	uitoa(pps, pps_str, sizeof(pps_str));
	strcat(pps_str, " Pixels/s");

	font = gdispOpenFont("UI2 Double");
	gdispClear(Black);
	gdispDrawStringBox(0, 0, width, 30, "GFX - Benchmark", font, White, justifyCenter);
	gdispDrawStringBox(0, height/2, width, 30, pps_str, font, White, justifyCenter);
	//gdispDrawString(20, height/2, pps_str, font, White);
}



int main(void) {
    
    //initDelay();
    
	gfxInit();
    gdispSetOrientation(GDISP_ROTATE_90);
	
	benchmark();
    
	while(TRUE) {
		gfxSleepMilliseconds(3000);
	benchmark();
	}

	return 0;
}


/*
int  main(){
    initDelay();
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;                  //LED1
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    gfxInit();

    while (1)
    {					  
   //     LCD_test();	  	
        GPIO_SetBits(GPIOB, GPIO_Pin_5);
        Delay(100);					   
        GPIO_ResetBits(GPIOB, GPIO_Pin_5);	     //V6 	   
        Delay(100);									

    }




}*/

