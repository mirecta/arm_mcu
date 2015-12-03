
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/cm3/nvic.h>

#include "ws2811.h"

//(1.2<us>/2.5<us>)*180<steps>   = 86
//(0.5<us>/2.5<us>)*180<steps>   = 36


#define HPWM 85
#define LPWM 34
#define BIT_BUFFER_SIZE ((LED_COUNT * 8 * 3) + 1)

Color line[LED_COUNT];
uint8_t bitsBuffer[BIT_BUFFER_SIZE];


volatile static int transfer_state = 0;
static uint8_t hslrgbState = 0; //default in RGB mode


void hslToRgb(uint8_t h, uint8_t s, uint8_t l, uint8_t *r,uint8_t *g, uint8_t *b)
{
    uint8_t region, remainder, p, q, t;

    if (s == 0)
    {
        *r = l;
        *g = l;
        *b = l;
        return ;
    }

    region = h / 43;
    remainder = (h - (region * 43)) * 6; 

    p = (l * (255 - s)) >> 8;
    q = (l * (255 - ((s * remainder) >> 8))) >> 8;
    t = (l * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

    switch (region)
    {
        case 0:
            *r = l; *g = t; *b = p;
            break;
        case 1:
            *r = q; *g = l; *b = p;
            break;
        case 2:
            *r = p; *g = l; *b = t;
            break;
        case 3:
            *r = p; *g = q; *b = l;
            break;
        case 4:
            *r = t; *g = p; *b = l;
            break;
        default:
            *r = l; *g = p; *b = q;
            break;
    }

}


void rgbToHsl(uint8_t r,uint8_t g, uint8_t b, uint8_t *h, uint8_t *s, uint8_t *l)
{
    uint8_t rgbMin, rgbMax;

    rgbMin = r < g ? (r < b ? r : b) : (g < b ? g : b);
    rgbMax = r > g ? (r > b ? r : b) : (g > b ? g : b);

    *l = rgbMax;
    if (*l == 0)
    {
        *h = 0;
        *s = 0;
        return ;
    }

    *s = 255 * long(rgbMax - rgbMin) / *l;
    if (*s == 0)
    {
        *h = 0;
        return ;
    }

    if (rgbMax == r)
        *h = 0 + 43 * (g - b) / (rgbMax - rgbMin);
    else if (rgbMax == g)
        *h = 85 + 43 * (b - r) / (rgbMax - rgbMin);
    else
        *h = 171 + 43 * (r - g) / (rgbMax - rgbMin);

}




void initTimer4(void){
    
    rcc_periph_clock_enable(RCC_TIM4);

    timer_reset(TIM4);
    timer_set_mode(TIM4, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
    timer_set_prescaler(TIM4, 0);
    timer_continuous_mode(TIM4);
    timer_set_period(TIM4, 179); /* 72000000 / 400000 (400khz pwm) -1 */
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
    
}

void initDMA1_1(void){

    rcc_periph_clock_enable(RCC_DMA1);

    dma_channel_reset(DMA1, DMA_CHANNEL1);
    //dma_enable_circular_mode(DMA1, DMA_CHANNEL1); 
    dma_set_priority(DMA1, DMA_CHANNEL1, DMA_CCR_PL_VERY_HIGH );

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
   timer_disable_counter(TIM4);
   gpio_toggle(GPIOE, GPIO5); 
    dma_disable_channel(DMA1,DMA_CHANNEL1);
    dma_set_number_of_data(DMA1, DMA_CHANNEL1, BIT_BUFFER_SIZE); 
    transfer_state = 0;
    DMA_IFCR(DMA1) |= DMA_IFCR_CGIF1;
}

void ws2811Init( uint8_t hslrgb){
 
    hslrgbState = hslrgb;
     
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
   //start timer
   timer_enable_counter(TIM4);
   //transfer state
   transfer_state=1;

}

uint8_t ws2811IsReady(){

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

    uint8_t r,g,b;


    //GRB
    uint32_t offset = 0;
    for(int i = 0; i < LED_COUNT; ++i){
        if (hslrgbState == 0){
            r = line[i].a;
            g = line[i].b;
            b = line[i].c;
        }else{
            hslToRgb(line[i].a,line[i].b,line[i].c, &r,&g,&b);
        }

        setBits(offset + 0,r);
        setBits(offset + 8,g);
        setBits(offset + 16,b);
        offset+=24;
    }
}
void ws2811Clear(uint8_t hslrgb)
{
    hslrgbState = hslrgb;

    for(int i = 0; i < LED_COUNT; ++i){
        line[i].a = 0;
        line[i].b = 0;
        line[i].c = 0;
    }

}

void ws2811Sync(void){
    setColors();
    bitSync();

}




void ws2811SetPixelHSL(int index, uint8_t h, uint8_t s, uint8_t l)
{
  if(hslrgbState == 1){
      line[index].a = h;
      line[index].b = s;
      line[index].c = l;

  }else{
      uint8_t r,g,b;
      hslToRgb(h, s, l, &r, &g, &b);
      line[index].a = r;
      line[index].b = g;
      line[index].c = b; 
  
  }

}

void ws2811SetPixelRGB(int index, uint8_t r, uint8_t g, uint8_t b)
{

  if(hslrgbState == 0){
      line[index].a = r;
      line[index].b = g;
      line[index].c = b;

  }else{
  
      uint8_t h,s,l;
      rgbToHsl(r, g, b, &h, &s, &l);
      line[index].a = h;
      line[index].b = s;
      line[index].c = l;

    }
}


void ws2811RotateLeft(void){


    uint8_t a,b,c;
    a = line[0].a;
    b = line[0].b;
    c = line[0].c;

    for (int i = 1; i < LED_COUNT; ++i){
        line[i-1].a = line[i].a;
        line[i-1].b = line[i].b;
        line[i-1].c = line[i].c;
    }
        line[LED_COUNT - 1].a = a;
        line[LED_COUNT - 1].b = b;
        line[LED_COUNT - 1].c = c;
   
}

void ws2811RotateRight(void){

    uint8_t a,b,c;
        a = line[LED_COUNT - 1].a;
        b = line[LED_COUNT - 1].b;
        c = line[LED_COUNT - 1].c;

    for (int i = LED_COUNT - 1; i >= 1; --i){
        line[i].a = line[i-1].a;
        line[i].b = line[i-1].b;
        line[i].c = line[i-1].c;
    }
   
    line[0].a = a;
    line[0].b = b;
    line[0].c = c;

}

