


#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include "ws2812.h"




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
   ws2812Init(0);

   

for(int i = 0; i < 30; ++i)
{

    ws2812SetPixelHSL(i,i*8,240,i);
}

   ws2812Sync();
             delay(5000);


while (1) {
            while(!ws2812IsReady());
             ws2812RotateRight();
             
            delay(200000);
             ws2812Sync();


            /* Manually: */
            // GPIOC_BSRR = GPIO12;     /* LED off */
            // for (i = 0; i < 800000; i++) /* Wait a bit. */
            //  __asm__("nop");
            // GPIOC_BRR = GPIO12;      /* LED on */
            // for (i = 0; i < 800000; i++) /* Wait a bit. */
            //  __asm__("nop");

            /* Using API functions gpio_set()/gpio_clear(): */
            // gpio_set(GPIOC, GPIO12); /* LED off */
            // for (i = 0; i < 800000; i++) /* Wait a bit. */
            //  __asm__("nop");
            // gpio_clear(GPIOC, GPIO12);   /* LED o

//             gpio_toggle(GPIOE, GPIO5);
            // delay(5000000);


}



}
