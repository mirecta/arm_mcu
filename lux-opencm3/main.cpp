


#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>


#include "ee_printf.h"



void delay( const uint32_t loop){
    for (uint32_t i = 0; i < loop; i++)    /* Wait a bit. */
        __asm__("nop");

}


void clock_setup(void){
    rcc_clock_setup_in_hse_8mhz_out_72mhz();
    rcc_periph_clock_enable(RCC_GPIOC);
}

void gpio_setup(void){


    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_50_MHZ,
                          GPIO_CNF_OUTPUT_PUSHPULL, GPIO5);

}


void serial_setup(){

    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_USART2);

    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
            GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART2_TX);

    /* Setup UART parameters. */
    usart_set_baudrate(USART2, 9600);
    usart_set_databits(USART2, 8);
    usart_set_stopbits(USART2, USART_STOPBITS_1);
    usart_set_mode(USART2, USART_MODE_TX);
    usart_set_parity(USART2, USART_PARITY_NONE);
    usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);

    /* Finally enable the USART. */
    usart_enable(USART2);

}



void serial_printf(const char* fmt, ...){


    char buf[100];
    va_list args;
    const char *c = buf;

    va_start(args, fmt);
    ee_vsprintf(buf, fmt, args);
    va_end(args);



    while(*c){
        usart_send_blocking(USART2, *c);
    ++c;
    }
}


int main(void){

    clock_setup();
    gpio_setup();
    gpio_set(GPIOC, GPIO5);
    serial_setup();



    while (1) {
        delay(2000000);
        serial_printf("Kurva \n \r");
        gpio_toggle(GPIOC,GPIO5);
    }

}
