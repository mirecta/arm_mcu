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


void initDebug(int x, int y, int width, int height){
#ifdef DEBUG            
    font_t font;
	font = gdispOpenFont("UI2");

    GWindowInit     wi;
    gwinClearInit(&wi);
    wi.show = TRUE;
    wi.x = x; 
    wi.y = y; 
    wi.width = width; 
    wi.height = height;
    GWDEBUG = gwinConsoleCreate(0, &wi);
	gwinSetColor(GWDEBUG, White);
	gwinSetBgColor(GWDEBUG,  HTML2COLOR(0x1f1f1f));
	gwinClear(GWDEBUG);
    gwinSetFont(GWDEBUG, font);
#endif //DEBUG

}


#ifdef __cplusplus
 }
#endif

