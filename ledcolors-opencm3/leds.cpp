


#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <stdlib.h>

#include "ws2812.h"


#define MAX_L 50

uint8_t updown[LED_COUNT];
uint8_t hue = 0;
uint8_t saturation = 250;


void delay( const uint32_t loop){
    for (uint32_t i = 0; i < loop; i++)    /* Wait a bit. */
        __asm__("nop");

}


void clock_setup(void){
    rcc_clock_setup_in_hse_8mhz_out_72mhz();
    rcc_periph_clock_enable(RCC_GPIOE);
}

void gpio_setup(void){


    gpio_set_mode(GPIOE, GPIO_MODE_OUTPUT_50_MHZ,
                          GPIO_CNF_OUTPUT_PUSHPULL, GPIO5);

}

int main(void){

    clock_setup();
    gpio_setup();
    gpio_set(GPIOE, GPIO5);
    ws2812Init(1);


    int changed = 0;

    for(int i = 0; i < LED_COUNT; ++i)
    {

        ws2812SetPixelHSL(i,hue/*rand()%256*/,240,rand()%MAX_L);
        updown[i]=rand()%2;

    }

    ws2812Sync();
    delay(5000);


    while (1) {
        //           ws2812RotateRight();

        for(int i = 0; i < LED_COUNT; ++i)
        {
            if(updown[i] == 0){
                if(line[i].c == 0){
                    line[i].a = hue;
                    line[i].b = saturation;
                    changed++;
                    ++line[i].c;
                    updown[i] = 1;
                }else{
                    --line[i].c;
                }
            }else{
                if(line[i].c == MAX_L){
                    --line[i].c;
                    updown[i] = 0;
                }else{
                    ++line[i].c;
                }

            }
        }
        if (changed >= 2*LED_COUNT){
           // hue +=32;
           hue = (rand()%8)*32;
            saturation=250;
            changed = 0;
        }

        while(!ws2812IsReady());
        delay(200000);
        ws2812Sync();
    }

}
