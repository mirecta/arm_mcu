
#include <inttypes.h>


#ifndef _LCDI2C_H_
#define _LCDI2C_H_

#define LCD_CMD 0
#define LCD_DATA 1

class LcdI2c {
    public:
    LcdI2c(uint8_t addr, uint8_t en, uint8_t rw, uint8_t rs, uint8_t bl);
    void setBacklight(uint8_t value);
    void init(void);
    void clear(void);
    void gotoxy(uint8_t x, uint8_t y);
    
    const char* print(const char *string);
    const char* print(const char *string, uint8_t size);      
  
    private:
      void data8(const uint8_t data);
      void send4(const uint8_t data, const uint8_t type);
      void cmd8(const uint8_t cmd);
    

      void i2c_write_port(const uint8_t data);
      void delay(const uint32_t loop);
    private:
      uint8_t addr, en, rw, rs, bl;
      uint8_t pins;


};

#endif
