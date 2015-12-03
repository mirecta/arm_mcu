


#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>






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
                          GPIO_CNF_OUTPUT_PUSHPULL, GPIO5|GPIO4|GPIO6);

}

int main(void){

    clock_setup();
    gpio_setup();
    gpio_set(GPIOE, GPIO5);




    while (1) {
        delay(2000000);
        gpio_toggle(GPIOE,GPIO5|GPIO6);
    }

}
