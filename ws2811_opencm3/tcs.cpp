#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/exti.h>


#include <stdio.h>

#include "tcs.h"


/*
   PA0 - S0
   PA1 - S1
   PA2 - OE
   PA3 - S2
   PA4 - S3
   PA5 - ON
   PA6 - TIM1_CH1
*/


volatile static uint8_t measure_complete = 1;
volatile static uint16_t measure_pulse_count;

void _delay( const uint32_t loop){
    for (uint32_t i = 0; i < loop; i++)    /* Wait a bit. */
        __asm__("nop");

}



void initTimer3(){
//set timer at max speed
    rcc_periph_clock_enable(RCC_TIM3);

    timer_reset(TIM3);
    timer_set_mode(TIM3, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
    timer_set_prescaler(TIM3, 1200); //60kHz
    timer_one_shot_mode(TIM3);
    timer_set_period(TIM3, 60000 - 1); //just right 1s
    timer_enable_preload(TIM3);

    //enable IRQ request on overflow
    timer_enable_irq(TIM3, TIM_DIER_UIE);    
    nvic_enable_irq(NVIC_TIM3_IRQ);

}

void tim3_isr(){


        timer_clear_flag(TIM3,TIM_SR_UIF);
        gpio_toggle(GPIOE,GPIO6);
        measure_complete = 1;
        exti_disable_request(EXTI6);
}

void exti9_5_isr(){
    if (measure_complete == 0)
        ++measure_pulse_count;

       exti_reset_request(EXTI6);
}


void startMeasure(){
    measure_complete = 0;
    measure_pulse_count = 0;
    timer_enable_counter(TIM3);
    exti_enable_request(EXTI6);
}

uint8_t isMeasureDone(){
       return measure_complete;
}

void tcsInit(void){
    
    rcc_periph_clock_enable(RCC_GPIOA);
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
                          GPIO_CNF_OUTPUT_PUSHPULL, GPIO0 | GPIO1 | GPIO2 | GPIO3 | GPIO4 | GPIO5);
    gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT , GPIO6);
//disable output
    gpio_set(GPIOA,GPIO2);
//set exti channel 6 
    exti_select_source(EXTI6,GPIOA);  
    exti_set_trigger(EXTI6,EXTI_TRIGGER_RISING);
    nvic_enable_irq(NVIC_EXTI9_5_IRQ);

    initTimer3();
}

void tcsOn(void){
    gpio_set(GPIOA,GPIO5);
}

void tcsOff(void){
     gpio_clear(GPIOA,GPIO5);
}


uint8_t measureValue(){
    _delay(6000);
    startMeasure();
    while(!isMeasureDone());
    
    printf(" %d \n",measure_pulse_count);
    
    return 255*  measure_pulse_count / 800;
}

void tcsGetRGB(uint8_t *r, uint8_t *g, uint8_t *b){


    //set freq scale
     gpio_clear(GPIOA, GPIO0);
     gpio_set(GPIOA,   GPIO1);
    //measure intensity
    //enable output
     gpio_clear(GPIOA,GPIO2);



    //RED
    gpio_clear(GPIOA, GPIO3|GPIO4);
    *r = measureValue();
    printf("r:%d \n",*r);
    //GREEN
    gpio_set(GPIOA, GPIO3|GPIO4);
    *g = measureValue();
    printf("g:%d \n",*g);

    //BLUE
    gpio_clear(GPIOA, GPIO3);
    *b = measureValue();
    printf("b:%d \n",*b);
    //CLEAR
    gpio_set(GPIOA, GPIO3);
    gpio_clear(GPIOA, GPIO4);

   


    //disable output
     gpio_set(GPIOA,GPIO2);
    //off freq
   gpio_clear(GPIOA, GPIO0|GPIO1);

}

