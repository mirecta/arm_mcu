

#include <libopencm3/stm32/spi.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#include "disp.h"


void disp_delay( const uint32_t loop){
    for (uint32_t i = 0; i < loop; i++)    /* Wait a bit. */
        __asm__("nop");

}
void spi1_setup(){
    /* Reset SPI, SPI_CR1 register cleared, SPI is disabled */
    spi_reset(SPI1);

    /* Set up SPI in Master mode with:
     * Clock baud rate: 1/64 of peripheral clock frequency
     * Clock polarity: Idle Low
     * Clock phase: Data valid on 1st clock pulse
     * Data frame format: 16-bit
     * Frame format: MSB First
     */
    spi_init_master(SPI1, SPI_CR1_BAUDRATE_FPCLK_DIV_64, SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
            SPI_CR1_CPHA_CLK_TRANSITION_1, SPI_CR1_DFF_16BIT, SPI_CR1_MSBFIRST);

    /*
     * Set NSS management to software.
     *
     * Note:
     * Setting nss high is very important, even if we are controlling the GPIO
     * ourselves this bit needs to be at least set to 1, otherwise the spi
     * peripheral will not send any data out.
     */
    spi_enable_software_slave_management(SPI1);
    spi_set_nss_high(SPI1);

     spi_enable_ss_output(SPI1);
    /* Enable SPI1 periph. */
    spi_enable(SPI1);
}

void disp_send(uint16_t data){
    gpio_clear(GPIOA,GPIO3);
    spi_send(SPI1,data);

    gpio_set(GPIOA,GPIO3);
    disp_delay(500);
}


void dispBrightness(uint8_t br){
      disp_send(0x0a00 | br);
}


void dispInit(){
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_AFIO);
    rcc_periph_clock_enable(RCC_SPI1);

    /* Configure GPIOs: SS=PA4, SCK=PA5, MISO=PA6 and MOSI=PA7 */
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
            GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO4 | GPIO5 |GPIO7 );
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,GPIO_CNF_OUTPUT_PUSHPULL, GPIO3);

    gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO6);

    gpio_set(GPIOA,GPIO3);

    spi1_setup();

    disp_send(0x0c01);
    disp_send(0x09ff);
    disp_send(0x0af5);
    disp_send(0x0b07);
    for(int i = 0; i < 9 ; ++i)
        disp_send((i << 8) | 0x0f);


}

void dispPrintNumber(int number){
    uint8_t sign = 0;

    if (number == 0){
        disp_send(0x0100);
        for(int i = 2; i  < 9; ++i){
            disp_send((i << 8) | 0x0f);
        }
        return;
    }
    if (number < 0){
        sign = 1;
        number = - number;
    }
    if (number > 99999999)
        return;

    for (int i = 1; i < 9; ++i){


        if( number > 0){
            disp_send((i << 8) | number % 10);
            number /= 10;
        }else{
            if(sign){
                disp_send((i << 8) | 0x0a);
                sign = 0;
            }else{
                disp_send((i << 8) | 0x0f);
            }
        }
    }
}

