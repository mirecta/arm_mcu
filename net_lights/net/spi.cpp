
#include <libopencm3/stm32/spi.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#include "spi.h"



void ENC28J60_CSL(void) {
    gpio_clear(GPIOB,GPIO11);
}

void ENC28J60_CSH(void) {
    gpio_set(GPIOB,GPIO11);
}

void ENC28J60_SPI_Init(void) 
{ 
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_AFIO);
    rcc_periph_clock_enable(RCC_SPI2);


    /* Configure GPIOs: SS=PB12, SCK=PB13, MISO=PB14 and MOSI=PB15 */
   
    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
            GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO12 | GPIO13 |GPIO15 );
    /* CS = PB11*/
    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,GPIO_CNF_OUTPUT_PUSHPULL, GPIO11);

    gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO14);

    //gpio_set(GPIOB,GPIO11);
    

    /* Reset SPI, SPI_CR1 register cleared, SPI is disabled */
    spi_reset(SPI2);

    /* Set up SPI in Master mode with:
     * Clock baud rate: 1/4 of peripheral clock frequency
     * Clock polarity: Idle Low
     * Clock phase: Data valid on 1st clock pulse
     * Data frame format: 8-bit
     * Frame format: MSB First
     */
    spi_init_master(SPI2, SPI_CR1_BAUDRATE_FPCLK_DIV_4, SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
            SPI_CR1_CPHA_CLK_TRANSITION_1, SPI_CR1_DFF_8BIT, SPI_CR1_MSBFIRST);

    /*
     * Set NSS management to software.
     *
     * Note:
     * Setting nss high is very important, even if we are controlling the GPIO
     * ourselves this bit needs to be at least set to 1, otherwise the spi
     * peripheral will not send any data out.
     */
    spi_enable_software_slave_management(SPI2);
    spi_set_nss_high(SPI2);

    spi_enable_ss_output(SPI2);
    /* Enable SPI periph. */
    spi_enable(SPI2);
    

} 

//SPI transfer 
uint8_t	ENC28J60_ReadWrite(uint8_t writedat) 
{ 

    return spi_xfer(SPI2, writedat);
    
} 

