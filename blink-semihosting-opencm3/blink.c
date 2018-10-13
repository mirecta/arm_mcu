/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2010 Gareth McMullin <gareth@blacksphere.co.nz>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>

extern void  initialise_monitor_handles(void);
#define STEP 10
//timer3 channel 4
void delay(){
    int i;
for (i = 0; i < 50000; i++)    /* Wait a bit. */
            __asm__("nop");
}
int main(void)
{
    int i;

    rcc_clock_setup_in_hse_8mhz_out_72mhz();
    rcc_periph_clock_enable(RCC_GPIOC);
    rcc_periph_clock_enable(RCC_TIM3);
    
    rcc_periph_clock_enable(RCC_AFIO);
    gpio_primary_remap(0,AFIO_MAPR_TIM3_REMAP_FULL_REMAP );
    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_50_MHZ,
            GPIO_CNF_OUTPUT_ALTFN_PUSHPULL , GPIO9);

    timer_set_mode(TIM3, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE,
               TIM_CR1_DIR_UP);
    
    timer_set_prescaler(TIM3,36);
    timer_set_period(TIM3, 1000);
    timer_set_oc_mode(TIM3, TIM_OC4, TIM_OCM_PWM1);
    timer_enable_oc_output(TIM3, TIM_OC4);
   // timer_enable_preload(TIM3);
    //timer_continuous_mode(TIM3);
    timer_enable_counter(TIM3);
    
    


#if defined(ENABLE_SEMIHOSTING) && (ENABLE_SEMIHOSTING) 
    initialise_monitor_handles();
#endif
    uint16_t a = 0;
    while (1)
    {
        for(i=0; i< 1000; i++){
        timer_set_oc_value(TIM3, TIM_OC4, i);
        delay();
        }
        for(i=999; i >=0 ; i--){
        timer_set_oc_value(TIM3, TIM_OC4, i);
        delay();
        }       
        //gpio_toggle(GPIOC, GPIO9); /* LED on/off */
    //printf("hello world\n");
        
        
    }
}

