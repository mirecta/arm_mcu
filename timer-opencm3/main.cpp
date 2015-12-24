/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2010 Gareth McMullin <gareth@blacksphere.co.nz>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */


/*help


keycodes http://www.mindrunway.ru/IgorPlHex/USBKeyScan.pdf
out endpoint  https://github.com/openstenoproject/stenosaurus/blob/master/bootloader/usb.c

  */


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>

#include "lcdi2c.h"



extern void  initialise_monitor_handles(void);




LcdI2c lcd(0x20,4,5,6,7);



void i2c_setup(){
   
    /* Enable clocks for I2C2 and AFIO. */

    /* Set alternate functions for the SCL and SDA pins of I2C2. */
    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
            GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN,
            GPIO7 | GPIO6);

    /* Disable the I2C before changing any configuration. */
    i2c_peripheral_disable(I2C1);

    /* APB1 is running at 36MHz. */
    i2c_set_clock_frequency(I2C1, I2C_CR2_FREQ_36MHZ);

    /* 400KHz - I2C Fast Mode */
    i2c_set_fast_mode(I2C1);
  
    /*
     * fclock for I2C is 36MHz APB2 -> cycle time 28ns, low time at 400kHz
     * incl trise -> Thigh = 1600ns; CCR = tlow/tcycle = 0x1C,9;
     * Datasheet suggests 0x1e.
     */
    i2c_set_ccr(I2C1, 0x1e);

    /*
     * fclock for I2C is 36MHz -> cycle time 28ns, rise time for
     * 400kHz => 300ns and 100kHz => 1000ns; 300ns/28ns = 10;
     * Incremented by 1 -> 11.
     */
    //  i2c_set_trise(I2C1, 0x0b);

//      i2c_set_own_7bit_slave_address(I2C1, 0x32);  
      i2c_set_dutycycle(I2C1, I2C_CCR_DUTY_DIV2);
      i2c_disable_ack(I2C1);

    //addressing mode
    i2c_peripheral_enable(I2C1);
}


void clock_setup(){
    rcc_clock_setup_in_hse_8mhz_out_72mhz();
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_I2C1);
    rcc_periph_clock_enable(RCC_AFIO);
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOE);
}

void gpio_setup(){

    gpio_set_mode(GPIOE, GPIO_MODE_OUTPUT_50_MHZ,GPIO_CNF_OUTPUT_PUSHPULL, GPIO5);
   
       

    //gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN , GPIO15| GPIO14| GPIO13| GPIO12);

    //gpio_set(GPIOB,GPIO15| GPIO14| GPIO13| GPIO12);
}


int main(void)
{
    clock_setup();
    i2c_setup();
    gpio_setup();

    lcd.init();

#if defined(ENABLE_SEMIHOSTING) && (ENABLE_SEMIHOSTING) 
    initialise_monitor_handles();
#endif
    
    lcd.clear();
    lcd.delay(20000); 
    lcd.setBacklight(1);
    lcd.gotoxy(0,0);
    lcd.printf("sss %02d",2);
    /*lcd.print(" Casovac  ");
    lcd.gotoxy(0,1);
    lcd.print("       Test      ");
    lcd.gotoxy(0,2);
    lcd.print("   Test  ");
    lcd.gotoxy(0,3);
    lcd.print("       v1.0         ");*/
   
	while (1);
}
/*
int _write(int file, char *ptr, int len){


    switch (file) {
        case STDOUT_FILENO:
        case STDERR_FILENO:
            lcd.print(ptr,len);
            gpio_toggle(GPIOE,GPIO5);
            break;

        default:
            errno = EBADF;
            return -1;

    }
    return len;
}
*/
