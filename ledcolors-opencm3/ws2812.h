
#ifndef _WS2812_H_
#define _WS2812_H_


//https://github.com/Daedaluz/stm32-ws2812/blob/master/src/ws2812.c
#define LED_COUNT 30



struct  Color {
    
    Color( uint8_t _r, uint8_t _g, uint8_t _b):r(_r),g(_g),b(_b){}

    Color():r(0),g(0),b(0){}

    uint8_t r;
    uint8_t g;
    uint8_t b;
} __attribute__((packed));


extern Color line[LED_COUNT];


void ws2812Init(void);
void ws2812Sync(void);
uint8_t ws2812IsReady(void);
void ws2812Clear(void);


#endif //_WS2812_H_
