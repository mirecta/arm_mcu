/*
*
* Copyright (C) Patryk Jaworski <regalis@regalis.com.pl>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
*/
#include <stm32f10x.h>
 
/* Helpers for SystemInitError() */
#define SYSTEM_INIT_ERROR_FLASH 0x01
#define SYSTEM_INIT_ERROR_PLL 0x02
#define SYSTEM_INIT_ERROR_CLKSRC 0x04
#define SYSTEM_INIT_ERROR_HSI 0x08


void SystemInitError(uint8_t error_source);


void SystemInit() {
    /* Enable Power Control clock */
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;
  /* Set HSION bit */
  RCC->CR |= (uint32_t)0x00000001;
  while ((RCC->CR & RCC_CR_HSIRDY) == 0);

  /* Reset SW, HPRE, PPRE1, PPRE2, ADCPRE and MCO bits */
  RCC->CFGR &= (uint32_t)0xF0FF0000;
    


  /* Reset HSEON, CSSON and PLLON bits */
  RCC->CR &= (uint32_t)0xFEF6FFFF;

  /* Reset HSEBYP bit */
  RCC->CR &= (uint32_t)0xFFFBFFFF;

  /* Reset PLLSRC, PLLXTPRE, PLLMUL and USBPRE/OTGFSPRE bits */
  RCC->CFGR &= (uint32_t)0xFF80FFFF;


    // Enable HSE oscilator 8Mhz
    RCC->CR |= RCC_CR_HSEON;
    /* Wait until HSE ready */
    while ((RCC->CR & RCC_CR_HSERDY) == 0);
 

    FLASH->ACR |= FLASH_ACR_PRFTBE;
    FLASH->ACR &= ~(FLASH_ACR_LATENCY);
    FLASH->ACR |= FLASH_ACR_LATENCY_2;
    /* Check flash latency */
    if ((FLASH->ACR & FLASH_ACR_LATENCY) != FLASH_ACR_LATENCY_2) {
        SystemInitError(SYSTEM_INIT_ERROR_FLASH);
    }

    /* HCLK = SYSCLK */
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
    /* PCLK2 = HCLK */
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;
    /* PCLK1 = HCLK */
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
 
    /*
     * Configure Main PLL
     * HSE as clock input
     * PLL mult by 9
     * APB1 div 2
     * USB div by 1.5
     * sys clock still HSI
     */
    RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE |
                                                    RCC_CFGR_PLLMULL);
    RCC->CFGR |= RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMULL9;
 
    /* PLL On */
    RCC->CR |= RCC_CR_PLLON;
    /* Wait until PLL is locked */
    while ((RCC->CR & RCC_CR_PLLRDY) == 0);
 
 
    
    RCC->CFGR &= ~(RCC_CFGR_SW);

    /* Set clock source to PLL */
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    /* Check clock source */
    while ((RCC->CFGR & RCC_CFGR_SWS_PLL) != RCC_CFGR_SWS_PLL);
 
 
}
 
void SystemInitError(uint8_t error_source) {
    while(1);
}
