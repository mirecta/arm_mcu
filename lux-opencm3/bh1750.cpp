

#include "bh1750.h"




#define BH_POWER_DOWN 0x00
#define BH_POWER_UP   0x01
#define BH_RESET      0x07


#define BH_HIRES1     0x00
#define BH_HIRES2     0x01
#define BH_LORES      0x03

#define BH_CONT       0x10
#define BH_OT         0x20


#define BH_MT_HI     0x40
#define BH_MT_LO     0x60




BH1750::BH1750(uint32_t i2c,  uint8_t address, uint8_t continuous, uint8_t resolution)
    :i2c(i2c),address(address),mode(0x00){

    if(continuous)
        mode = BH_CONT;
    else
        mode = BH_OT;

    switch(resolution){

            default:
            case 0x00:
                mode |= BH_LORES;
                break;
            case 0x01:
                mode |= BH_HIRES1;
                break;
            case 0x02:
                mode |= BH_HIRES2;
                break;
    }
}



void BH1750::init(){

    iic_write(BH_POWER_UP);
    iic_write(mode);
    iic_write(BH_RESET);
}

void BH1750::setMeasureTime(uint8_t time){

    iic_write(BH_MT_HI | (time >> 5));
    iic_write(BH_MT_LO | (time & 0x1f));  
}


int BH1750::getLux(){

    if((mode >> 1) == 1){
        return iic_read16();
    }else{
        iic_write(mode);
        return iic_read16();
    }
}


void BH1750::iic_write(uint8_t data){

    uint32_t reg32 __attribute__((unused));

    i2c_send_start(i2c);
    /* Waiting for START is send and switched to master mode. */

    while (!((I2C_SR1(i2c) & I2C_SR1_SB)
                & (I2C_SR2(i2c) & (I2C_SR2_MSL | I2C_SR2_BUSY))));

    /* Send destination address. */
    i2c_send_7bit_address(i2c, address , I2C_WRITE);

    /* Waiting for address is transferred. */
    while (!(I2C_SR1(i2c) & I2C_SR1_ADDR));

    /* Cleaning ADDR condition sequence. */
    reg32 = I2C_SR2(i2c);

    /* Sending the data. */
    i2c_send_data(i2c, data); /* stts75 config register */
    while (!(I2C_SR1(i2c) & (I2C_SR1_BTF | I2C_SR1_TxE)));
    //while (!(I2C_SR1(i2c) & I2C_SR1_BTF));                                                                                                                                                                                                                                  
    /* Send STOP condition. */
    i2c_send_stop(i2c);


}

uint16_t BH1750::iic_read16(){


    //special case read two bytes
    uint16_t value;
    uint32_t reg32 __attribute__((unused));

    /* Send START condition. */
    i2c_send_start(i2c);

    /* Waiting for START is send and switched to master mode. */
    while (!((I2C_SR1(i2c) & I2C_SR1_SB)
                & (I2C_SR2(i2c) & (I2C_SR2_MSL | I2C_SR2_BUSY))));

    /* Say to what address we want to talk to. */
    i2c_send_7bit_address(i2c, address, I2C_READ); 

    /* 2-byte receive is a special case. See datasheet POS bit. */
    I2C_CR1(i2c) |= (I2C_CR1_POS | I2C_CR1_ACK);

    /* Waiting for address is transferred. */
    while (!(I2C_SR1(i2c) & I2C_SR1_ADDR));

    /* Cleaning ADDR condition sequence. */
    reg32 = I2C_SR2(i2c);

    /* Cleaning I2C_SR1_ACK. */
    I2C_CR1(i2c) &= ~I2C_CR1_ACK;

    /* Now the slave should begin to send us the first byte. Await BTF. */
    while (!(I2C_SR1(i2c) & I2C_SR1_BTF));
    value = (uint16_t)(I2C_DR(i2c) << 8); /* MSB */

    /*
     * Yes they mean it: we have to generate the STOP condition before
     * saving the 1st byte.
     */
    I2C_CR1(i2c) |= I2C_CR1_STOP;

    value |= I2C_DR(i2c); /* LSB */

    /* Original state. */
    I2C_CR1(i2c) &= ~I2C_CR1_POS;

    return value;

}



