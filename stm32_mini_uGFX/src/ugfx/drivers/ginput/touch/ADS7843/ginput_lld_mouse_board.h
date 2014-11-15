/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */


#ifndef _GINPUT_LLD_MOUSE_BOARD_H
#define _GINPUT_LLD_MOUSE_BOARD_H

#include "stm32f10x.h"
// PB7 - CS
// PB6 - INT pen
// PA6 - MISO
// PA7 - MOSI
// PA5 - SCK

static inline void init_board(void)
{
    //init CS and INT pen 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_GPIOB , ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;                  //CS
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;              //INT pen
    //GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    

    GPIO_SetBits(GPIOB, GPIO_Pin_7);

    //enable spi clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);  

    //enable gpio alternate func for spi
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //enable whole spi 
    SPI_InitTypeDef   SPI_InitStructure;
    
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);
    SPI_Cmd(SPI1, ENABLE);


}

static inline bool_t getpin_pressed(void)
{
	//read pen interrupt
    return (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6));
}

static inline void aquire_bus(void)
{
	//CS LOW
    GPIO_ResetBits(GPIOB, GPIO_Pin_7);
}

static inline void release_bus(void)
{
	//CS UP
    GPIO_SetBits(GPIOB, GPIO_Pin_7);
}

static inline uint16_t read_value(uint16_t port)
{
    static uint8_t txbuf;
    static uint8_t rxbuf[2] = {0};
    uint16_t ret;

    txbuf = port;


    SPI_I2S_SendData(SPI1,txbuf);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE 
                            | SPI_I2S_FLAG_RXNE | SPI_I2S_FLAG_BSY ) == RESET); 
   
    SPI_I2S_ReceiveData(SPI1);  
    
    SPI_I2S_SendData(SPI1,0);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE 
                            | SPI_I2S_FLAG_RXNE | SPI_I2S_FLAG_BSY ) == RESET); 
    
    rxbuf[0] = SPI_I2S_ReceiveData(SPI1);
   
    SPI_I2S_SendData(SPI1,0);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE 
                            | SPI_I2S_FLAG_RXNE | SPI_I2S_FLAG_BSY ) == RESET); 
    
    rxbuf[1] = SPI_I2S_ReceiveData(SPI1);
    
    ret = (rxbuf[0] << 5) | (rxbuf[1] >> 3); 
	return ret;
}

#endif /* _GINPUT_LLD_MOUSE_BOARD_H */
    
