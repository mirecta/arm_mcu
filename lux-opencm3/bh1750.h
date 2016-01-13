
#include <libopencm3/stm32/i2c.h>


#ifndef _BH1750_H_

#define _BH1750_H_


#define BH_ADDR_L 0x23
#define BH_ADDR_H 0x5c


class BH1750 {


    public:
        BH1750(uint32_t i2c = I2C1, uint8_t address = BH_ADDR_L, uint8_t continuous = 0, uint8_t resolution = 0);
        void init(void);
        void setMeasureTime(uint8_t time);
        int getLux(void);

    private:
        void iic_write(uint8_t data);
        uint16_t iic_read16();

    private:
        uint32_t i2c;
         uint8_t address;
         uint8_t mode;


};











#endif // _BH1750_H_
