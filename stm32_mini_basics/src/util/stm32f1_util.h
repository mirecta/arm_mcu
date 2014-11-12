#ifndef _STM32F_UTIL_H
#define _STM32F_UTIL_H

#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif

void SysTick_Handler(void);

int initDelay();
     

uint32_t GetMilis();

void Delay(volatile uint32_t d);


#ifdef __cplusplus
 }
#endif



#endif //_STM32F_UTIL_H

