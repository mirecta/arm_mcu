


#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>


#include "ee_printf.h"
#include "bh1750.h"


BH1750 bh1750(I2C1,BH_ADDR_L,1,1);

void delay( const uint32_t loop){
    for (uint32_t i = 0; i < loop; i++)    /* Wait a bit. */
        __asm__("nop");

}


void clock_setup(void){
    rcc_clock_setup_in_hse_8mhz_out_72mhz();
    rcc_periph_clock_enable(RCC_GPIOC);
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_AFIO);
    
    rcc_periph_clock_enable(RCC_I2C1);   
}

void gpio_setup(void){


    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_50_MHZ,
                          GPIO_CNF_OUTPUT_PUSHPULL, GPIO5);

}

void i2c_setup(){

    /* Enable clocks for I2C2 and AFIO. */

    /* Set alternate functions for the SCL and SDA pins of I2C2. */
    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
            GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN,
            GPIO7 | GPIO6);

    /* Disable the I2C before changing any configuration. */
    i2c_peripheral_disable(I2C1);

    /* APB1 is running at 36MHz. */
    i2c_set_clock_frequency(I2C1, I2C_CR2_FREQ_36MHZ);

    /* 400KHz - I2C Fast Mode */
    i2c_set_fast_mode(I2C1);

    /*
     * fclock for I2C is 36MHz APB2 -> cycle time 28ns, low time at 400kHz
     * incl trise -> Thigh = 1600ns; CCR = tlow/tcycle = 0x1C,9;
     * Datasheet suggests 0x1e.
     */
    i2c_set_ccr(I2C1, 0x1e);

    /*
     * fclock for I2C is 36MHz -> cycle time 28ns, rise time for
     * 400kHz => 300ns and 100kHz => 1000ns; 300ns/28ns = 10;
     * Incremented by 1 -> 11.
     */
      i2c_set_trise(I2C1, 0x0b);

    //      i2c_set_own_7bit_slave_address(I2C1, 0x32);  
    i2c_set_dutycycle(I2C1, I2C_CCR_DUTY_DIV2);
    i2c_disable_ack(I2C1);

    //addressing mode
    i2c_peripheral_enable(I2C1);
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

    int val = 0;
    clock_setup();
    gpio_setup();
    gpio_set(GPIOC, GPIO5);
    i2c_setup();
    serial_setup();
    bh1750.init();


    while (1) {
        delay(1000000);
        val=bh1750.getLux();
        serial_printf("%d\n\r",val);
        gpio_toggle(GPIOC,GPIO5);
    }

}
