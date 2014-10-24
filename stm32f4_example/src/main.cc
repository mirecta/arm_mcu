/*
*
* Copyright (C) Miroslav Talasek <miroslav.talasek@seznam.cz>
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
#include <stm32f4xx.h>
 
#define LED_PIN 5
#define LED_ON() GPIOA->BSRRL |= (1 << 5)
#define LED_OFF() GPIOA->BSRRH |= (1 << 5)

volatile int status = 0; 

#ifdef __cplusplus
 extern "C" {
#endif

void TIM2_IRQHandler(void) {
  // flash on update event
  if (TIM2->SR & TIM_SR_UIF){
  if(status == 0) {
    LED_ON();
    status = 1;
  }else{
   LED_OFF();
   status = 0;
 }
  }
   
  TIM2->SR = 0x0; // reset the status register
}

#ifdef __cplusplus
 }
#endif

//try systick 



int main() {
    /* Enbale GPIOA clock */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    /* Configure GPIOA pin 5 as output */
    GPIOA->MODER |= GPIO_MODER_MODER5_0;
    /* Configure GPIOA pin 5 in max speed */
    GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR5;
 

    //enable timer clock
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    //enable interrupt
    NVIC_EnableIRQ(TIM2_IRQn);

    TIM2->PSC = 0x0; // no prescaler, timer counts up in max speed
    TIM2->DIER |= TIM_DIER_UIE; // enable update interrupt
    TIM2->ARR = 25000000; // count to this value
    TIM2->CR1 |= TIM_CR1_ARPE | TIM_CR1_CEN | TIM_CR1_URS; // autoreload on, counter enabled, event only in owerflow or auto reload
    TIM2->EGR = TIM_EGR_UG; // trigger update event to reload timer registers

    LED_ON();
  while(1);
    /* Turn on the LED */
 // LED_ON();
 
}
