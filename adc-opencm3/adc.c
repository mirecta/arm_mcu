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
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>


/*PA0,PA1,PA2,PA3*/


extern void  initialise_monitor_handles(void);

volatile uint16_t result_array_raw[4];
volatile int16_t result_array[4];

void wait(){
    int i;
    for (i = 0; i < 800000; i++)    /* Wait a bit. */
        __asm__("nop");
    
}

void nvic_setup(){
    nvic_enable_irq(NVIC_HARD_FAULT_IRQ);
   // nvic_set_priority(NVIC_HARD_FAULT_IRQ, 0);

    //nvic_enable_irq(NVIC_EXTI0_IRQ);
    //nvic_set_priority(NVIC_EXTI0_IRQ, 1);

   // nvic_enable_irq(NVIC_USART1_IRQ);
   // nvic_set_priority(NVIC_USART1_IRQ, 2);

    nvic_enable_irq(NVIC_DMA1_CHANNEL1_IRQ);
 //   nvic_set_priority(NVIC_DMA1_CHANNEL1_IRQ, 3);

}

void adc_setup(){
    uint8_t channel_array[16];

    /* make shure it didnt run during config */
    adc_off(ADC1);

    adc_enable_scan_mode(ADC1); /*scan mode means we scan all channels of the group to the end */

    adc_set_continuous_conversion_mode(ADC1); /* means we scan the group the whole day til someone disable continous mode */
    
    //adc_set_single_conversion_mode(ADC1); /* this means we scan the group only one time */

   adc_disable_discontinuous_mode_regular(ADC1); /* discontinous mode means all channels in a group will be *
                                                  * scanned in fragments of discmod channel count */


  //  adc_disable_external_trigger_regular(ADC1);

    adc_enable_external_trigger_regular(ADC1, ADC_CR2_EXTSEL_SWSTART);
    adc_set_right_aligned(ADC1);
    adc_set_sample_time_on_all_channels(ADC1, ADC_SMPR_SMP_7DOT5CYC);

    /* enable adc */
    adc_on(ADC1);

    /* wait for adc starting up*/
    wait();
    adc_reset_calibration(ADC1);
    adc_calibration(ADC1);


    /* Select the channel(s) (up  to 16) we want to convert. */
    channel_array[0] = 0; // 1
    channel_array[1] = 1;
    channel_array[2] = 2;
    channel_array[3] = 3; // 4


    adc_set_regular_sequence(ADC1, 4, channel_array);

    /* we want to use it with dma */
    adc_enable_dma(ADC1);
   wait();
    //start adc
    adc_start_conversion_regular(ADC1);

}

void dma_setup(){

    //ADC
    /* no reconfig for every ADC group conversion */
    dma_enable_circular_mode(DMA1, DMA_CHANNEL1);
    /* the memory pointer has to be increased, and the peripheral not */
    dma_enable_memory_increment_mode(DMA1, DMA_CHANNEL1);
    /* ADC_DR is only 16bit wide in this mode */
    dma_set_peripheral_size(DMA1, DMA_CHANNEL1, DMA_CCR_PSIZE_16BIT);
    /*destination memory is also 16 bit wide */
    dma_set_memory_size(DMA1, DMA_CHANNEL1, DMA_CCR_MSIZE_16BIT);
    /* direction is from ADC to memory */
    dma_set_read_from_peripheral(DMA1, DMA_CHANNEL1);
    /* get the data from the ADC data register */
    dma_set_peripheral_address(DMA1, DMA_CHANNEL1,(uint32_t) &ADC_DR(ADC1));
    /* put everything in this array */
    dma_set_memory_address(DMA1, DMA_CHANNEL1, (uint32_t) &result_array_raw);
    /* we convert only 3 values in one adc-group */
    dma_set_number_of_data(DMA1, DMA_CHANNEL1, 4);
    /* we want an interrupt after the 3 halfwords. so we can compute something afterwards. */
    dma_enable_transfer_complete_interrupt(DMA1, DMA_CHANNEL1);

    /* dma ready to go. waiting til the peripheral gives the first data */
    dma_enable_channel(DMA1, DMA_CHANNEL1);
    //END ADC

}

void dma1_channel1_isr(){

    /* clear the interrupt flag */
    DMA_IFCR(DMA1) |= DMA_IFCR_CGIF1;
}


void clock_setup(){
    rcc_clock_setup_in_hse_8mhz_out_72mhz();
    rcc_periph_clock_enable(RCC_GPIOC);
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_ADC1);
    rcc_periph_clock_enable(RCC_DMA1);
    rcc_periph_clock_enable(RCC_AFIO);
}

void gpio_setup(){
    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ,GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
    //ADC
    gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_ANALOG, GPIO0);
    gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_ANALOG, GPIO1);
    gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_ANALOG, GPIO2);
    gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_ANALOG, GPIO3);
}

int main(void)
{
    int i;

    clock_setup();
    gpio_setup();

    dma_setup(); // adc done interrupt
    adc_setup(); // 4 channels

    nvic_setup();

#if defined(ENABLE_SEMIHOSTING) && (ENABLE_SEMIHOSTING) 
    initialise_monitor_handles();
#endif
    while (1)
    {
        gpio_toggle(GPIOC, GPIO13); /* LED on/off */
        for (i = 0; i < 800000; i++)    /* Wait a bit. */
            __asm__("nop");

        printf("ADC: %d,%d,%d,%d\n",result_array_raw[0],result_array_raw[1],result_array_raw[2],result_array_raw[3]);
    }
}

