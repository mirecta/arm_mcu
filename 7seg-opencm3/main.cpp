

#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/timer.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#include "disp.h"




void delay( const uint32_t loop){
    for (uint32_t i = 0; i < loop; i++)    /* Wait a bit. */
        __asm__("nop");

}


void clock_setup(void){
    rcc_clock_setup_in_hse_8mhz_out_72mhz();
    rcc_periph_clock_enable(RCC_GPIOC);
    rcc_periph_clock_enable(RCC_GPIOE);
   
    rcc_periph_clock_enable(RCC_GPIOA);
    
}

void gpio_setup(void){
    gpio_set_mode(GPIOC, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO0 | GPIO1);
    //i want pullup to A8, A9, A10, A07
    gpio_set(GPIOC,GPIO0 | GPIO1);

    gpio_set_mode(GPIOE, GPIO_MODE_OUTPUT_50_MHZ,GPIO_CNF_OUTPUT_PUSHPULL, GPIO5|GPIO6);



    gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO0 | GPIO1);
    //i want pullup to A0, A1
    gpio_set(GPIOA,GPIO0 | GPIO1);
    

}

void init_tim2(){
    rcc_periph_clock_enable(RCC_TIM2); 
    timer_reset(TIM2);
    timer_set_period(TIM2, 0xFFFF);
    timer_slave_set_mode(TIM2,TIM_SMCR_SMS_EM2);
    timer_ic_set_input(TIM2, TIM_IC1, TIM_IC_IN_TI1);
    timer_ic_set_input(TIM2, TIM_IC2, TIM_IC_IN_TI2);
    timer_set_oc_polarity_high(TIM2, TIM_OC1);
    timer_set_oc_polarity_low(TIM2, TIM_OC2);
    timer_ic_set_filter(TIM2, TIM_IC1, TIM_IC_DTF_DIV_16_N_8  );
    timer_ic_set_filter(TIM2, TIM_IC2, TIM_IC_DTF_DIV_16_N_8  );
    timer_enable_counter(TIM2);
//    timer_set_counter(TIM2,32768);
}


int main(void){

    clock_setup();
    gpio_setup();
    gpio_set(GPIOE, GPIO5);
init_tim2(); 
    dispInit();
    uint32_t portc;
    uint8_t enable=0;
    int a = 0;


    while (1) {
        portc = (~gpio_port_read(GPIOC));
        if(portc &0x02)
            enable = ~enable;

        if (a > 99999999)
            a = 0;

        dispPrintNumber( (int16_t) (timer_get_counter(TIM2)));
        ++a;

        delay(200000);
        if(enable)
            gpio_toggle(GPIOE,GPIO5|GPIO6);
    }

}
