


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
   ws2812Init();

   line[0].r = 128;
   line[0].g = 0;
   line[0].b = 0;
   line[15].r = 0;
   line[15].g = 128;
   line[15].b = 0;
   line[29].r = 0;
   line[29].g = 0;
   line[29].b = 128;

   ws2812Sync();


while (1) {
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
             delay(5000000);


}



}
