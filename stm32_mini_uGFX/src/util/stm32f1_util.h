#ifndef _STM32F_UTIL_H
#define _STM32F_UTIL_H

#include <stdint.h>

#ifdef DEBUG
#include "gfx.h"
#include <stdarg.h>

GHandle GWDEBUG;

 #define DBG(fmt, ...) \
    gwinPrintf(GWDEBUG, "%s:%d: " fmt, __FILE__, \
            __LINE__, ##__VA_ARGS__);
#else

#define DBG(fmt, ...)

#endif //DEBUG

#ifdef __cplusplus
 extern "C" {
#endif

void SysTick_Handler(void);

int initDelay();
     

uint32_t GetMilis();

void Delay(volatile uint32_t d);

void initDebug(int x, int y, int width, int height);


#ifdef __cplusplus
 }
#endif



#endif //_STM32F_UTIL_H

