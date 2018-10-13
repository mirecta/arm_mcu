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

#include <stdio.h>
#include <stdlib.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include <string.h>
#include <fcntl.h>

#include <assert.h>

#include "fatfs/xprintf.h"
#include "fatfs/ff.h"
#include "fatfs/diskio.h"



extern "C" void  initialise_monitor_handles(void);
extern "C" void disk_timerproc (void);

FATFS fs;

void hwInit(){
    rcc_clock_setup_in_hse_8mhz_out_72mhz();
    rcc_periph_clock_enable(RCC_GPIOC);
    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ,
            GPIO_CNF_OUTPUT_PUSHPULL, GPIO9);
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_AFIO);
    rcc_periph_clock_enable(RCC_SPI2);


    /* Configure GPIOs: SS=PB12, SCK=PB13, MISO=PB14 and MOSI=PB15 */
   
    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
            GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO12 | GPIO13 |GPIO15 );
    /* CS = PB11*/

    gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO14);
	/* Enable Systick for benchmark timing */
	/* 72MHz / 8 => 9000000 counts per second */
	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB_DIV8); 
	/* 9000000/9000 = 1000 overflows per second - every 1ms one interrupt */
	systick_set_reload(8999);
	systick_interrupt_enable();
	/* start counting */
	systick_counter_enable();
}


void listDir(char *path){

    FRESULT res;
    DIR dir;
    UINT i;
    static FILINFO fno;
    res = f_opendir(&dir, path);
    if (res == FR_OK) {
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
                printf("%s\n",fno.fname);
        }
        f_closedir(&dir);
    }
}

int main(void)
{

    hwInit();
#if defined(ENABLE_SEMIHOSTING) && (ENABLE_SEMIHOSTING) 
    initialise_monitor_handles();
#endif


    f_mount(&fs, "", 0);
    listDir("/");

    while (1)
    {
       // gpio_toggle(GPIOC, GPIO9); /* LED on/off */
       //printf("hello world\n");
        for (int i = 0; i < 2400000; i++)    /* Wait a bit. */
            __asm__("nop");
    }
}

void sys_tick_handler()
{
	static int temp32;

	temp32++;


	/* we call this handler every 1ms so 1000ms = 1s on/off */
	if (temp32 == 1000) {
		gpio_toggle(GPIOC, GPIO9); /* LED2 on/off */
		temp32 = 0;
	}
    disk_timerproc();
}

