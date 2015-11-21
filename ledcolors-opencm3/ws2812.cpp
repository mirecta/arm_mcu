
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/cm3/nvic.h>

#include "ws2812.h"

//(0.70<us>/1.25<us>)*90<steps>   = 50
//(0.35<us>/1.25<us>)*90<steps>   = 25


#define HPWM 50
#define LPWM 25
#define BIT_BUFFER_SIZE ((LED_COUNT * 8 * 3) + 1)

Color line[LED_COUNT];
uint8_t bitsBuffer[BIT_BUFFER_SIZE];


volatile int transfer_state = 0;


void initTimer4(void){
    
    rcc_periph_clock_enable(RCC_TIM4);

    timer_reset(TIM4);
    timer_set_mode(TIM4, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
    timer_set_prescaler(TIM4, 0);
    timer_continuous_mode(TIM4);
    timer_set_period(TIM4, 89); /* 72000000 / 800000 (800khz pwm) -1 */
    timer_disable_oc_output(TIM4, TIM_OC1);
    timer_disable_oc_clear(TIM4, TIM_OC1);
    timer_enable_oc_preload(TIM4, TIM_OC1);
    timer_set_oc_slow_mode(TIM4, TIM_OC1);
    timer_set_oc_mode(TIM4, TIM_OC1, TIM_OCM_PWM1);
    timer_set_oc_polarity_high(TIM4, TIM_OC1);
    timer_set_oc_value(TIM4, TIM_OC1, 0);
    timer_enable_oc_output(TIM4, TIM_OC1);
    timer_enable_preload(TIM4);

    //enable DMA request on compare
    timer_enable_irq(TIM4, TIM_DIER_CC1DE);
//    timer_enable_irq(TIM4, TIM_DIER_UDE);
    
    timer_set_dma_on_compare_event(TIM4);
   //start timer
   timer_enable_counter(TIM4);
    
}

void initDMA1_1(void){

    rcc_periph_clock_enable(RCC_DMA1);

    dma_channel_reset(DMA1, DMA_CHANNEL1);
    //dma_enable_circular_mode(DMA1, DMA_CHANNEL1); 
    dma_set_priority(DMA1, DMA_CHANNEL1, DMA_CCR_PL_HIGH );

    /* the memory pointer has to be increased, and the peripheral not */
    dma_enable_memory_increment_mode(DMA1, DMA_CHANNEL1);
    dma_disable_peripheral_increment_mode(DMA1, DMA_CHANNEL1);
    
    dma_set_peripheral_size(DMA1, DMA_CHANNEL1, DMA_CCR_PSIZE_16BIT);
    dma_set_memory_size(DMA1, DMA_CHANNEL1, DMA_CCR_MSIZE_8BIT);
    dma_set_read_from_memory(DMA1, DMA_CHANNEL1);
    
    dma_set_peripheral_address(DMA1, DMA_CHANNEL1,(uint32_t) &TIM4_CCR1);
    dma_set_memory_address(DMA1, DMA_CHANNEL1, (uint32_t) &bitsBuffer[0]);
    dma_set_number_of_data(DMA1, DMA_CHANNEL1, BIT_BUFFER_SIZE);
    
    dma_enable_transfer_complete_interrupt(DMA1, DMA_CHANNEL1);
    
    nvic_enable_irq(NVIC_DMA1_CHANNEL1_IRQ);


}  

void dma1_channel1_isr(void){
   gpio_toggle(GPIOE, GPIO5); 
    dma_disable_channel(DMA1,DMA_CHANNEL1);
    dma_set_number_of_data(DMA1, DMA_CHANNEL1, BIT_BUFFER_SIZE); 
    transfer_state = 0;
    DMA_IFCR(DMA1) |= DMA_IFCR_CGIF1;
}

void ws2812Init(){
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_AFIO);

    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
                          GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO6);

    initTimer4();
    initDMA1_1();
}


void bitSync(){

    //start dma
   dma_enable_channel(DMA1,DMA_CHANNEL1);
   //transfer state
   transfer_state=1;

}

uint8_t ws2812IsReady(){

    return transfer_state?0:1;
}


void setBits( uint32_t offset, uint8_t byte){
   
    bitsBuffer[offset + 0] = (byte & 0x80)? HPWM:LPWM;
    bitsBuffer[offset + 1] = (byte & 0x40)? HPWM:LPWM;
    bitsBuffer[offset + 2] = (byte & 0x20)? HPWM:LPWM;
    bitsBuffer[offset + 3] = (byte & 0x10)? HPWM:LPWM;
    bitsBuffer[offset + 4] = (byte & 0x08)? HPWM:LPWM;
    bitsBuffer[offset + 5] = (byte & 0x04)? HPWM:LPWM;
    bitsBuffer[offset + 6] = (byte & 0x02)? HPWM:LPWM;
    bitsBuffer[offset + 7] = (byte & 0x01)? HPWM:LPWM;
  
}


void setColors(void){
    //GRB
    uint32_t offset = 0;
    for(int i = 0; i < LED_COUNT; ++i){
        setBits(offset + 0,line[i].g);
        setBits(offset + 8,line[i].r);
        setBits(offset + 16,line[i].b);
        offset+=24;
    }
}
void ws2812Clear(void)
{
    for(int i = 0; i < LED_COUNT; ++i){
        line[i].g = 0;
        line[i].r = 0;
        line[i].b = 0;
    }

}


void ws2812Sync(void){
    setColors();
    bitSync();

}





