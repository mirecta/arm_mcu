#include <stdint.h>

#ifndef _GLIBM_H_
#define _GLIBM_H_

#define RGB(r,g,b) (((b >> 3) << 11) | ((g >> 2) << 5) | ((r>> 3)))

class GlibM{
    
public:
   GlibM(int16_t w, int16_t h);
  

   void clearScr(uint16_t color = 0);


   void setTextColor(uint16_t color) { textcolor = color; }
   
   void setTextColor(uint16_t color, uint16_t bgcolor) { textcolor = color; textbgcolor = bgcolor;} 

   void setTextSize(uint8_t size) { textsize = size;}

   void drawCircle(int16_t x, int16_t y, int16_t r, uint16_t color); 

   void drawCircleHelper( int16_t x, int16_t y,
           int16_t r, uint8_t cornername, uint16_t color);

   void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
   void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
   void fillCircle(int16_t x, int16_t y, int16_t r, uint16_t color);
   void fillCircleHelper(int16_t x, int16_t y, int16_t r,
        uint8_t cornername, int16_t delta, uint16_t color);
   void drawLine(int16_t x0, int16_t y0,int16_t x1, int16_t y1,
        uint16_t color);
   void drawRect(int16_t x, int16_t y,int16_t w, int16_t h, uint16_t color);
    
   void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

   void drawRoundRect(int16_t x, int16_t y, int16_t w,
           int16_t h, int16_t r, uint16_t color) ;
    
   void fillRoundRect(int16_t x, int16_t y, int16_t w,
           int16_t h, int16_t r, uint16_t color);

   void write(const uint8_t c);

   void drawChar(int16_t x, int16_t y, const unsigned char c,
        uint16_t color, uint16_t bg, uint8_t size);

   void drawTextXY(int16_t x, int16_t y, const char* text);

   void printfXY(int16_t x, int16_t y, char* format, ...);
   
   void drawPixel(int16_t x, int16_t y, uint16_t color);

   
   void rectToFill(int16_t x1, int16_t x2, int16_t y1, int16_t y2);
   void rectFill(uint16_t color);

   uint16_t width(void) const {return _w;}
   uint16_t height(void) const {return _h;}

private:
       int16_t _w;
       int16_t _h;
       int16_t _cx;
       int16_t _cy;
       uint16_t textcolor;
       uint16_t textbgcolor;
       uint8_t  textsize;
       bool wrap;
};


#endif //_GLIBM_H_
