


#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <stdlib.h>
#include <stdio.h>

#include "ws2811.h"
#include "tcs.h"

#define MAX_L 30

uint8_t updown[LED_COUNT];
uint8_t hue = 0;
uint8_t saturation = 250;

extern "C"  void  initialise_monitor_handles(void);

void delay( const uint32_t loop){
    for (uint32_t i = 0; i < loop; i++)    /* Wait a bit. */
        __asm__("nop");

}


void clock_setup(void){
    rcc_clock_setup_in_hse_8mhz_out_72mhz();
    rcc_periph_clock_enable(RCC_GPIOE);
    //buttons
    rcc_periph_clock_enable(RCC_GPIOC);

}

void gpio_setup(void){


    gpio_set_mode(GPIOE, GPIO_MODE_OUTPUT_50_MHZ,
                          GPIO_CNF_OUTPUT_PUSHPULL, GPIO5|GPIO6);




    gpio_set_mode(GPIOC, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO0 | GPIO1);
    //i want pullup to A8, A9, A10, A07
    gpio_set(GPIOC,GPIO0 | GPIO1);



}

int main(void){

#if defined(ENABLE_SEMIHOSTING) && (ENABLE_SEMIHOSTING)
        initialise_monitor_handles();
#endif

    clock_setup();
    gpio_setup();
    gpio_set(GPIOE, GPIO5);
    ws2811Init(1);
    tcsInit();
    //    srand(5000);

    int changed = 0;


    tcsOn();
    uint8_t r,g,b;
    while (1) {
        tcsGetRGB(&r,&g,&b);
        // r=0; g=0; b= 0;
        ws2811SetPixelRGB(0,r,g,b);
        delay(300000);
        ws2811Sync();
        

        //           ws2811RotateRight();

     /* for(int i = 0; i < LED_COUNT; ++i)
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

        while(!ws2811IsReady());
        delay(300000/(MAX_L/30));
        ws2811Sync();*/
    }


}
