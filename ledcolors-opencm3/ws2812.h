
#ifndef _WS2812_H_
#define _WS2812_H_


//https://github.com/Daedaluz/stm32-ws2812/blob/master/src/ws2812.c
#define LED_COUNT 30



struct  Color {
    

    uint8_t a;
    uint8_t b;
    uint8_t c;
} __attribute__((packed));


extern Color line[LED_COUNT];


void ws2812Init(uint8_t hslrgb = 0);
void ws2812Sync(void);
uint8_t ws2812IsReady(void);
void ws2812Clear(uint8_t hslrgb = 0);

void ws2812SetPixelHSL(int index, uint8_t h, uint8_t s, uint8_t l);
void ws2812SetPixelRGB(int index, uint8_t r, uint8_t g, uint8_t b);
void ws2812RotateLeft(void);
void ws2812RotateRight(void);



#endif //_WS2812_H_
