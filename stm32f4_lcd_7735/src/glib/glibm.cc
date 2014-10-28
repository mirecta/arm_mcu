
#include "glibm.h"
#include "st7735.h"
#include "stm32f4_util.h"
#include "glcdfont.h"

#define swap(a, b) { int16_t t = a; a = b; b = t; }

namespace{ 
    int16_t abs(int16_t v){
        if (v < 0)
            return -v;
        else
            return v;
    }
}


GlibM::GlibM(int16_t w, int16_t h):_w(w),_h(h),wrap(true){

    Delay(10);
    st7735_Init();
    Delay(10);
    st7735_DisplayOn();
    Delay(10);
}


void GlibM::clearScr(uint16_t color){
    st7735_ResetArea();
    st7735_Clear(color);
}



void GlibM::drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color){

    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;
    st7735_WritePixel(x0 , y0+r, color);
    st7735_WritePixel(x0 , y0-r, color);
    st7735_WritePixel(x0+r, y0 , color);
    st7735_WritePixel(x0-r, y0 , color);
    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        st7735_WritePixel(x0 + x, y0 + y, color);
        st7735_WritePixel(x0 - x, y0 + y, color);
        st7735_WritePixel(x0 + x, y0 - y, color);
        st7735_WritePixel(x0 - x, y0 - y, color);
        st7735_WritePixel(x0 + y, y0 + x, color);
        st7735_WritePixel(x0 - y, y0 + x, color);
        st7735_WritePixel(x0 + y, y0 - x, color);
        st7735_WritePixel(x0 - y, y0 - x, color);
    }
}

void GlibM::drawCircleHelper( int16_t x0, int16_t y0,
        int16_t r, uint8_t cornername, uint16_t color){
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;
    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        if (cornername & 0x4) {
            st7735_WritePixel(x0 + x, y0 + y, color);
            st7735_WritePixel(x0 + y, y0 + x, color);
        }
        if (cornername & 0x2) {
            st7735_WritePixel(x0 + x, y0 - y, color);
            st7735_WritePixel(x0 + y, y0 - x, color);
        }
        if (cornername & 0x8) {
            st7735_WritePixel(x0 - y, y0 + x, color);
            st7735_WritePixel(x0 - x, y0 + y, color);
        }
        if (cornername & 0x1) {
            st7735_WritePixel(x0 - y, y0 - x, color);
            st7735_WritePixel(x0 - x, y0 - y, color);
        }
    }
}


void GlibM::drawFastVLine(int16_t x, int16_t y,
        int16_t h, uint16_t color) {
    st7735_SetArea(x,x,y,y+h-1);
    for (int i=0; i < h; ++i)
        st7735_WritePixelRaw(color);
}
void GlibM::drawFastHLine(int16_t x, int16_t y,
        int16_t w, uint16_t color) {
    st7735_SetArea(x,x+w-1,y,y);
    for (int i=0; i < w; ++i)
        st7735_WritePixelRaw(color);
}

void GlibM::fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
    drawFastVLine(x0, y0-r, 2*r+1, color);
    fillCircleHelper(x0, y0, r, 3, 0, color);
}

void GlibM::fillCircleHelper(int16_t x0, int16_t y0, int16_t r,
        uint8_t cornername, int16_t delta, uint16_t color) {
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;
    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        if (cornername & 0x1) {
            drawFastVLine(x0+x, y0-y, 2*y+1+delta, color);
            drawFastVLine(x0+y, y0-x, 2*x+1+delta, color);
        }
        if (cornername & 0x2) {
            drawFastVLine(x0-x, y0-y, 2*y+1+delta, color);
            drawFastVLine(x0-y, y0-x, 2*x+1+delta, color);
        }
    }
}

void GlibM::drawLine(int16_t x0, int16_t y0,
        int16_t x1, int16_t y1,
        uint16_t color) {
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        swap(x0, y0);
        swap(x1, y1);
    }
    if (x0 > x1) {
        swap(x0, x1);
        swap(y0, y1);
    }
    int16_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);
    int16_t err = dx / 2;
    int16_t ystep;
    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }
    for (; x0<=x1; x0++) {
        if (steep) {
            st7735_WritePixel(y0, x0, color);
        } else {
            st7735_WritePixel(x0, y0, color);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}


void GlibM::drawRect(int16_t x, int16_t y,int16_t w, int16_t h, uint16_t color) {
    drawFastHLine(x, y, w, color);
    drawFastHLine(x, y+h-1, w, color);
    drawFastVLine(x, y, h, color);
    drawFastVLine(x+w-1, y, h, color);
}

void GlibM::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {

    st7735_SetArea(x,x + w - 1 ,y,y + h - 1 );

    for(int i = 0; i < w*h; i++){
        st7735_WritePixelRaw(color);
    }
}

void GlibM::drawRoundRect(int16_t x, int16_t y, int16_t w,
        int16_t h, int16_t r, uint16_t color) {
    // smarter version
    drawFastHLine(x+r , y , w-2*r, color); // Top
    drawFastHLine(x+r , y+h-1, w-2*r, color); // Bottom
    drawFastVLine(x , y+r , h-2*r, color); // Left
    drawFastVLine(x+w-1, y+r , h-2*r, color); // Right
    // draw four corners
    drawCircleHelper(x+r , y+r , r, 1, color);
    drawCircleHelper(x+w-r-1, y+r , r, 2, color);
    drawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
    drawCircleHelper(x+r , y+h-r-1, r, 8, color);
}

void GlibM::fillRoundRect(int16_t x, int16_t y, int16_t w,
        int16_t h, int16_t r, uint16_t color) {
    // smarter version
    fillRect(x+r, y, w-2*r, h, color);
    // draw four corners
    fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
    fillCircleHelper(x+r , y+r, r, 2, h-2*r-1, color);
}
void GlibM::write(uint8_t c) {
    if (c == '\n') {
        _cy += textsize*8;
        _cx = 0;
    } else if (c == '\r') {
        // skip em
    } else {
        drawChar(_cx, _cy, c, textcolor, textbgcolor, textsize);
        _cx += textsize*6;
        if (wrap && (_cx > (_w - textsize*6))) {
            _cy += textsize*8;
            _cx = 0;
        }
    }
}
// Draw a character
void GlibM::drawChar(int16_t x, int16_t y, unsigned char c,
        uint16_t color, uint16_t bg, uint8_t size) {
    if((x >= _w) || // Clip right
            (y >= _w) || // Clip bottom
            ((x + 6 * size - 1) < 0) || // Clip left
            ((y + 8 * size - 1) < 0)) // Clip top
        return;
    
    st7735_SetArea(x,x + 6 * size - 1,y,y + 8 * size - 1);

    for (int8_t i=0; i<6; i++ ) {
        uint8_t line;
        if (i == 5)
            line = 0x0;
        else
            line = font[(c*5)+i];
        
        uint8_t tmp = line;
        
        for (int8_t k = 0; k < size; ++k){
            line = tmp;
            for (int8_t j = 0; j<8; j++) {
                
                if (line & 0x1) {
                
                    if (size == 1) // default size
                        st7735_WritePixelRaw(color);
                    else { // big size
                        for (int8_t l = 0; l < size; ++l)
                            st7735_WritePixelRaw(color);
                    }
                } else  {
                    
                    if (size == 1) // default size
                        st7735_WritePixelRaw(bg);
                    else { // big size
                        for (int8_t l = 0; l < size; ++l)
                            st7735_WritePixelRaw(bg);
                    }
                }
                line >>= 1;
            }
        }
    }
}
