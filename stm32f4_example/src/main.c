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
//#include "stm32f4_gpio.h"
#include "stm32f4_util.h"

#define LED_PIN 5
#define LED_ON() GPIOA->BSRRL |= (1 << 5)
#define LED_OFF() GPIOA->BSRRH |= (1 << 5)



#ifdef __cplusplus
 extern "C" {
#endif
//----------------------------- uncoment to blink with interrupt ------------------
/*

  volatile int status = 0; 
  
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
*/
 //------------------------------------/blink with interrupt -----------------------
#ifdef __cplusplus
 }
#endif




int main() {
    initDelay();
    
    
    /* Enbale GPIOA clock */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    /* Configure GPIOA pin 5 in max speed */
    GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR5;
    //enable timer clock
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;


//-------------------------- uncoment to blink with interrupt ----------------
  /*
    
    GPIOA->MODER |= GPIO_MODER_MODER5_0; // A5 to ouput
     
    //enable interrupt
    NVIC_EnableIRQ(TIM2_IRQn);

    TIM2->PSC = 0x0; // no prescaler, timer counts up in max speed
    TIM2->DIER |= TIM_DIER_UIE; // enable update interrupt
    TIM2->ARR = 25000000; // count to this value
    TIM2->CR1 |= TIM_CR1_ARPE | TIM_CR1_CEN | TIM_CR1_URS; // autoreload on, counter enabled, event only in owerflow or auto reload
    TIM2->EGR = TIM_EGR_UG; // trigger update event to reload timer registers

    while(1);
*/
//-----------------------------------------------------------------------------
//------------------------------uncoment to pwm blink -------------------------
    
    GPIOA->MODER |= GPIO_MODER_MODER5_1; // A5 to alternate func
    GPIOA->AFR[0] |= 1 << 20; // enable AF1 to A5 1 << (5*4)
    
    //setup timer 2
    
    TIM2->PSC = 50;
    TIM2->ARR = 10000;
    TIM2->CR1 |= TIM_CR1_ARPE | TIM_CR1_CEN; //enable timer2 , preload autoreload
    TIM2->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1PE | TIM_CCMR1_OC1FE; //enable PWM1 on CCR1 with fast compare and preload 
    TIM2->EGR |= TIM_EGR_UG; // refresh registers
    TIM2->CCER |= TIM_CCER_CC1E; // enable CC1E basic output with basic polarity 
    TIM2->CCR1 = 5000; //set pwm to half max is ARR-1
   
  while(1){
    for (int i = 0 ; i< 10000; i+=20){
      TIM2->CCR1 = i;
      Delay(1);
    }
    for (int i = 10000 ; i >=0 ; i-=20){
      TIM2->CCR1 = i;
      Delay(1);
    }
    
  }
  
//------------------------------------------------------------------------------

}
