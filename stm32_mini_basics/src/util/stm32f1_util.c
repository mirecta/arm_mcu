#include <stm32f10x.h>
#include "stm32f1_util.h"

static volatile uint32_t milis;
#ifdef __cplusplus
 extern "C" {
#endif


     
void SysTick_Handler(void)
{
      milis++;
}

uint32_t GetMilis(){
    return milis;
}

//try systick 
void Delay(volatile uint32_t d)
{
    uint32_t tickstart = 0;
    tickstart = GetMilis();
    while((GetMilis() - tickstart) < d)
    {
    }
}

int initDelay(){
    milis=0;
    return SysTick_Config(CPU_FREQ/1000);
}


#ifdef __cplusplus
 }
#endif

