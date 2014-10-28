#include "st7735.h"
#include "stm32f4_util.h"
#include "stm32f4_gpio.h"

#include <stm32f4xx.h>

/* PINS
 * lcd_cs - PA0
 * lcd_rs - PA1
 * sd_cs - PA4
 * miso - PA6
 * mosi - PA7
 * sck - PA5
 * */



/* LCD IO functions */
void     LCD_IO_Init(void){

    //first of all enable clock to PA
    //and set it to high speed
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    GPIOA->OSPEEDR |= (GP_SPEED_HIGH << (GP_PIN0 << 1))
                    | (GP_SPEED_HIGH << (GP_PIN1 << 1))
                    | (GP_SPEED_HIGH << (GP_PIN4 << 1))
                    | (GP_SPEED_HIGH << (GP_PIN5 << 1))
                    | (GP_SPEED_HIGH << (GP_PIN6 << 1))
                    | (GP_SPEED_HIGH << (GP_PIN7 << 1));


    GPIOA->MODER |= (GP_MODE_OUTPUT << (GP_PIN0 << 1))
                  | (GP_MODE_OUTPUT << (GP_PIN1 << 1))
                  | (GP_MODE_OUTPUT << (GP_PIN4 << 1))
                  | (GP_MODE_ALTERNATE << (GP_PIN5 << 1))
                  | (GP_MODE_ALTERNATE << (GP_PIN6 << 1))
                  | (GP_MODE_ALTERNATE << (GP_PIN7 << 1));

    GPIOA->OTYPER |= (GP_TYPE_PUPD << GP_PIN0)
                    |(GP_TYPE_PUPD << GP_PIN1)
                    |(GP_TYPE_PUPD << GP_PIN4)
                    |(GP_TYPE_PUPD << GP_PIN5)
                    |(GP_TYPE_PUPD << GP_PIN6)
                    |(GP_TYPE_PUPD << GP_PIN7);

    GPIOA->PUPDR |= (GP_PULL_UP << (GP_PIN0 << 1))
                   |(GP_PULL_UP << (GP_PIN1 << 1))
                   |(GP_PULL_UP << (GP_PIN4 << 1))
                   |(GP_PULL_NONE << (GP_PIN5 << 1))//;
                   |(GP_PULL_NONE << (GP_PIN6 << 1))
                   |(GP_PULL_NONE << (GP_PIN7 << 1));

    GPIOA->AFR[0] |= (GP_AF5 << (GP_PIN5 << 2))
                   |(GP_AF5 << (GP_PIN6 << 2))
                   |(GP_AF5 << (GP_PIN7 << 2));

    //set lcd_cs to 1
    GPIOA->BSRRL = 1 << GP_PIN0;
    //set sd_cs to 1
    GPIOA->BSRRL = 1 << GP_PIN4;
    //now set whole spi 
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    // f/8 , CPOL = 0, CPHA = 0, MSTR = 1, DFF = 0, LSBFIRST=0, SPE = 1
    SPI1->CR1 |= SPI_CR1_BR_0 | SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI;//| SPI_CR1_CPOL | SPI_CR1_CPHA ;
    SPI1->CR1 |= SPI_CR1_SPE;

    Delay(100);   


}
void     LCD_IO_WriteData16(uint16_t data){
    //set data
    GPIOA->BSRRL = 1 << GP_PIN1;
    //reset cs
    GPIOA->BSRRH = 1 << GP_PIN0;
    
    //transmit data
    SPI1->DR = data >> 8;
    while(!(SPI1->SR & SPI_SR_TXE));
    //while(!(SPI1->SR & SPI_SR_RXNE));
    while((SPI1->SR & SPI_SR_BSY));
    
    SPI1->DR = data;
    while(!(SPI1->SR & SPI_SR_TXE));
    //while(!(SPI1->SR & SPI_SR_RXNE));
    while((SPI1->SR & SPI_SR_BSY));
    //set lcd_cs to 1
    
    GPIOA->BSRRL = 1 << GP_PIN0;
}
       

void     LCD_IO_WriteData(uint8_t Data){
    //set data
    GPIOA->BSRRL = 1 << GP_PIN1;
    //reset cs
    GPIOA->BSRRH = 1 << GP_PIN0;
    //transmit data
    SPI1->DR = Data;
    while(!(SPI1->SR & SPI_SR_TXE));
    //while(!(SPI1->SR & SPI_SR_RXNE));
    while((SPI1->SR & SPI_SR_BSY));
    //set lcd_cs to 1
    GPIOA->BSRRL = 1 << GP_PIN0;

}

void     LCD_IO_WriteReg(uint8_t Reg){
    //set command
    GPIOA->BSRRH = 1 << GP_PIN1;
    //reset cs
    GPIOA->BSRRH = 1 << GP_PIN0;
    //transmit data
    SPI1->DR = Reg;
    while(!(SPI1->SR & SPI_SR_TXE));
 //   while(!(SPI1->SR & SPI_SR_RXNE));
    while((SPI1->SR & SPI_SR_BSY));
    //set lcd_cs to 1
    GPIOA->BSRRL = 1 << GP_PIN0;

}

void     LCD_Delay(uint32_t delay){
        Delay(delay);
}



