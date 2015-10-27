#include "lcdi2c.h"


#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/rcc.h>

// LCD Commands
// ---------------------------------------------------------------------------
#define LCD_CLEARDISPLAY        0x01
#define LCD_RETURNHOME          0x02
#define LCD_ENTRYMODESET        0x04
#define LCD_DISPLAYCONTROL      0x08
#define LCD_CURSORSHIFT         0x10
#define LCD_FUNCTIONSET         0x20
#define LCD_SETCGRAMADDR        0x40
#define LCD_SETDDRAMADDR        0x80

// flags for display entry mode
// ---------------------------------------------------------------------------
#define LCD_ENTRYRIGHT          0x00
#define LCD_ENTRYLEFT           0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off and cursor control
// ---------------------------------------------------------------------------
#define LCD_DISPLAYON           0x04
#define LCD_DISPLAYOFF          0x00
#define LCD_CURSORON            0x02
#define LCD_CURSOROFF           0x00
#define LCD_BLINKON             0x01
#define LCD_BLINKOFF            0x00

// flags for display/cursor shift
// ---------------------------------------------------------------------------
#define LCD_DISPLAYMOVE         0x08
#define LCD_CURSORMOVE          0x00
#define LCD_MOVERIGHT           0x04
#define LCD_MOVELEFT            0x00

// flags for function set
// ---------------------------------------------------------------------------
#define LCD_8BITMODE            0x10
#define LCD_4BITMODE            0x00
#define LCD_2LINE               0x08
#define LCD_1LINE               0x00
#define LCD_5x10DOTS            0x04
#define LCD_5x8DOTS             0x00


LcdI2c::LcdI2c(uint8_t _addr, uint8_t _en, uint8_t _rw, uint8_t _rs, uint8_t _bl):
         addr(_addr),en( 1 << _en),rw( 1 << _rw),rs(1 << _rs),bl(1 << _bl),pins(0x00){
         
         }

void LcdI2c::delay( const uint32_t loop){
        for (uint32_t i = 0; i < loop; i++)    /* Wait a bit. */
            __asm__("nop");

}
void LcdI2c::init(void){

    send4(0x03,LCD_CMD);
    delay(100000);
    send4(0x03,LCD_CMD);
    delay(100000);
    send4(0x03,LCD_CMD);
    delay(100000);
    send4(0x02,LCD_CMD);
    delay(100000);

    
    cmd8(LCD_FUNCTIONSET |  LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS);
    delay(100000);
    cmd8(LCD_CURSORSHIFT | LCD_MOVERIGHT | LCD_CURSORMOVE );
    delay(100000);
    cmd8(LCD_DISPLAYCONTROL | LCD_DISPLAYON  | LCD_CURSOROFF | LCD_BLINKOFF );
    delay(100000);
    cmd8(LCD_RETURNHOME ) ;   
    delay(100000);
    cmd8(LCD_CLEARDISPLAY);
    delay(100000);

    setBacklight(1);


}

void LcdI2c::clear(void){
    cmd8(LCD_CLEARDISPLAY);
}
const char* LcdI2c::print(uint8_t line, const char *string){

//set row addr
    const uint8_t row_offsetsDef[]   = { 0x00, 0x40, 0x14, 0x54 }; // For regular LCDs
    const char *c = string;
    cmd8(LCD_SETDDRAMADDR | (row_offsetsDef[line]));
    
    while(*c)
        data8(*c++);
    
    return (c + 1); 
}


void LcdI2c::setBacklight(const uint8_t value){
 
    if(value)
        pins &= ~bl;
    else
        pins |= bl;
    i2c_write_port(pins);
}


void LcdI2c::data8(const uint8_t data){
    send4(data >> 4,LCD_DATA);
    send4(data & 0x0f,LCD_DATA);

}
void LcdI2c::send4(const uint8_t data, const uint8_t type){
        
        //clear data part
        pins &= 0xf0;
        
        if (type == LCD_DATA)
            pins |= rs;
        else
            pins &= ~rs;

        pins &= ~rw;
        pins |= data;
        pins |= en;
        i2c_write_port(pins);
    //    delay(500);
        pins &= ~en;
        i2c_write_port(pins);

}
void LcdI2c::cmd8(const uint8_t cmd){

    send4(cmd >> 4,LCD_CMD);
    send4(cmd & 0x0f,LCD_CMD);

}



void LcdI2c::i2c_write_port( const uint8_t data){
    uint32_t reg32 __attribute__((unused));

    i2c_send_start(I2C1);
    /* Waiting for START is send and switched to master mode. */
    
    while (!((I2C_SR1(I2C1) & I2C_SR1_SB)
                & (I2C_SR2(I2C1) & (I2C_SR2_MSL | I2C_SR2_BUSY))));
    
    /* Send destination address. */
    i2c_send_7bit_address(I2C1, addr , I2C_WRITE);

    /* Waiting for address is transferred. */
     while (!(I2C_SR1(I2C1) & I2C_SR1_ADDR));

    /* Cleaning ADDR condition sequence. */
    reg32 = I2C_SR2(I2C1);

    /* Sending the data. */
    i2c_send_data(I2C1, data); /* stts75 config register */
    while (!(I2C_SR1(I2C1) & (I2C_SR1_BTF | I2C_SR1_TxE)));
    //while (!(I2C_SR1(I2C1) & I2C_SR1_BTF));
    /* Send STOP condition. */
    i2c_send_stop(I2C1);
}
