/*
 * This file is part of the openfat project.
 *
 * Copyright (C) 2011  Department of Physics, University of Otago
 * Written by Gareth McMullin <gareth@blacksphere.co.nz>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
https://github.com/Laurenceb/STM32-Logger/blob/master/Util/fat_fs/src/sd_spi_stm32.c
/* MMC Card interface implementation.
 */

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "mmc.h"
#include "openfat/leaccess.h"

#define MMC_SECTOR_SIZE		512


static uint8_t spi_readwrite(uint32_t spi, uint8_t data)
{
    return spi_xfer(spi, data);

}

static void mmc_select(const struct mmc_port *mmc)
{
	gpio_clear(mmc->cs_port, mmc->cs_pin);
	/* Wait for not busy */
	while(spi_readwrite(mmc->spi, 0xFF) == 0);
}

static void mmc_release(const struct mmc_port *mmc)
{
	gpio_set(mmc->cs_port, mmc->cs_pin);
	/* Must cycle clock 8 times after CS is released */
	spi_readwrite(mmc->spi, 0xFF);
}

static void 
mmc_write_buffer(const struct mmc_port *mmc, const uint8_t *buf, int len)
{
	while(len--)
		spi_readwrite(mmc->spi, *buf++);
}

static void 
mmc_read_buffer(const struct mmc_port *mmc, uint8_t *buf, int len)
{
	while(len--)
		*buf++ = spi_readwrite(mmc->spi, 0xFF);
}

#define TOKEN_START_SINGLE_READ		0xFE
#define TOKEN_START_MULTI_READ		0xFE
#define TOKEN_START_SINGLE_WRITE	0xFE
#define TOKEN_START_MULTI_WRITE		0xFD
#define TOKEN_STOP_MULTI_WRITE		0xFC

static int
mmc_receive_block(const struct mmc_port *mmc, uint8_t *buf, int len)
{
	/* wait for token */
	while((*buf = spi_readwrite(mmc->spi, 0xFF)) == 0xFF);

	if(*buf != TOKEN_START_SINGLE_READ) /* Start data token */
		return -1;

	mmc_read_buffer(mmc, buf, len);

	/* Discard CRC bytes */
	spi_readwrite(mmc->spi, 0xFF);
	spi_readwrite(mmc->spi, 0xFF);
	
	return 0;

}

static int
mmc_transmit_block(const struct mmc_port *mmc, const uint8_t *buf, int len)
{
	/* wait for not busy */
	while(spi_readwrite(mmc->spi, 0xFF) != 0xFF);

	/* Send token */
	spi_readwrite(mmc->spi, TOKEN_START_SINGLE_WRITE);

	/* Sent data frame */
	mmc_write_buffer(mmc, buf, len);

	/* Send dummy CRC bytes */
	spi_readwrite(mmc->spi, 0xFF);
	spi_readwrite(mmc->spi, 0xFF);

	/* Return response code, zero on no error */
	return spi_readwrite(mmc->spi, 0xFF);
}

static uint8_t
mmc_command(const struct mmc_port *mmc, uint8_t cmd, uint32_t arg)
{
	uint8_t buf[6];
	uint8_t ret;

	if (cmd & 0x80){
		cmd &= 0x7F;
		ret = mmc_command(mmc,CMD55, 0);
		if (ret > 1) return ret;
	}
		

	buf[0] = cmd | 0x40;
	/* argument is packed big-endian */
	buf[1] = (arg >> 24) & 0xFF;
	buf[2] = (arg >> 16) & 0xFF;
	buf[3] = (arg >> 8) & 0xFF;
	buf[4] = arg & 0xFF;
        buf[5] = 0x01;  /* Dummy CRC + Stop */
	if (cmd == CMD0) buf[5] = 0x95; /* Valid CRC for CMD0(0) */
	if (cmd == CMD8) buf[5] = 0x87; /* Valid CRC for CMD8(0x1AA) */


	mmc_select(mmc);
	mmc_write_buffer(mmc, buf, sizeof(buf));
	
	if (cmd == CMD12) spi_readwrite(mmc->spi, 0xFF);

        int n = 10;

	do { /* Wait for response byte */
		ret = spi_readwrite(mmc->spi, 0xFF);
	} while((ret & 0x80) && --n);

	return ret;
}

static uint16_t mmc_get_sector_size(const struct block_device *dev)
{
	(void)dev;
	return MMC_SECTOR_SIZE;
}

static int mmc_read_sectors(const struct block_device *bldev,
		uint32_t sector, uint32_t count, void *buf)
{



	const struct mmc_port *mmc = (void*)bldev;
	uint32_t i;

	if (!(mmc->card_type & CT_BLOCK)) sector *= 512;	/* Convert to byte address if needed */
	mmc_select(mmc);

	if (count == 1) {	/* Single block read */
		if (mmc_command(mmc,CMD17, sector) == 0)	{ /* READ_SINGLE_BLOCK */
			if (mmc_receive_block(mmc,buf, 512)) {
				count = 0;
			}
		}
	}else {				/* Multiple block read */
		if (mmc_command(mmc,CMD18, sector) == 0) {	/* READ_MULTIPLE_BLOCK */
			do {
				if (!mmc_receive_block(mmc,buf, 512)) {
					break;
				}
				buf += 512;
			} while (--count);
			mmc_command(mmc,CMD12, 0);		/* STOP_TRANSMISSION */
		}
        }
	mmc_release(mmc);

	return count;
}

static int mmc_write_sectors(const struct block_device *bldev,
		uint32_t sector, uint32_t count, const void *buf)
{
	const struct mmc_port *mmc = (void*)bldev;
	uint32_t i;
 
	mmc_select(mmc);
	for(i = 0; i < count; i++) {
		mmc_command(mmc, MMC_WRITE_BLOCK, 
				(sector + i) * MMC_SECTOR_SIZE);
		mmc_transmit_block(mmc, buf, MMC_SECTOR_SIZE);
		buf += MMC_SECTOR_SIZE;
	}
	mmc_release(mmc);

	return i;
}

int 
mmc_init(uint32_t spi, uint32_t cs_port, uint16_t cs_pin, struct mmc_port *mmc)
{
	uint8_t ocr[4],cmd;
	int n;

	/* Intialise structure */
	memset(mmc, 0, sizeof(*mmc));

	/* Block device methods */
	mmc->bldev.get_sector_size = mmc_get_sector_size;
	mmc->bldev.read_sectors = mmc_read_sectors;
	mmc->bldev.write_sectors = mmc_write_sectors;

	mmc->spi = spi;
	mmc->cs_port = cs_port;
	mmc->cs_pin = cs_pin;

        spi_reset(mmc->spi);
	/* Do hardware init */
	/* Peripheral clocks must already be enabled.  
	 * SPI pins must already be configured. */
	spi_init_master(mmc->spi, 
			SPI_CR1_BAUDRATE_FPCLK_DIV_4, 
			SPI_CR1_CPOL_CLK_TO_1_WHEN_IDLE, 
			SPI_CR1_CPHA_CLK_TRANSITION_2, 
			SPI_CR1_DFF_8BIT,
			SPI_CR1_MSBFIRST);
	/* Ignore the stupid NSS pin */
	spi_enable_software_slave_management(mmc->spi);
	spi_set_nss_high(mmc->spi);

	spi_enable(mmc->spi);

	/* SD nCS pin init */
	gpio_set_mode(mmc->cs_port, GPIO_MODE_OUTPUT_50_MHZ, 
			GPIO_CNF_OUTPUT_PUSHPULL, mmc->cs_pin);

	/* Do card init ... */
	for(int i = 0 ; i < 80; i++) spi_readwrite(mmc->spi, 0xff);
	
	if (mmc_command(mmc,CMD0, 0) == 1) {			/* Enter Idle state */
		time_timeout = 1000;						/* Initialization timeout of 1000 milliseconds */
		if (mmc_command(mmc,CMD8, 0x1AA) == 1) {	/* SDHC */
			for (n = 0; n < 4; n++) ocr[n] = spi_readwrite(mmc->spi, 0xff);		/* Get trailing return value of R7 response */
			if (ocr[2] == 0x01 && ocr[3] == 0xAA) {				/* The card can work at VDD range of 2.7-3.6V */
				while (time_timeout && mmc_command(mmc,ACMD41, 1UL << 30));	/* Wait for leaving idle state (ACMD41 with HCS bit) */
				if (time_timeout && mmc_command(mmc,CMD58, 0) == 0) {		/* Check CCS bit in the OCR */
					for (n = 0; n < 4; n++) ocr[n] = spi_readwrite(mmc->spi, 0xff);
					mmc->card_type = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;
				}
			}
		} else {							/* SDSC or MMC */
			if (mmc_command(mmc,ACMD41, 0) <= 1) 	{
				mmc->card_type = CT_SD1; cmd = ACMD41;	/* SDSC */
			} else {
				mmc->card_type = CT_MMC; cmd = CMD1;	/* MMC */
			}
			while (time_timeout && mmc_command(mmc,cmd, 0));			/* Wait for leaving idle state */
			if (!time_timeout || mmc_command(mmc,CMD16, 512) != 0)	/* Set R/W block length to 512 */
				mmc->card_type = 0;
		}
	}
	
	if(mmc_command(mmc, MMC_GO_IDLE_STATE, 0) != 1) 
		return -1; /* Can't reset card? */
if(1) {
                uint8_t csd[16];
                if(mmc_command(mmc, MMC_SEND_CSD, 0) != 0)
                        return -1; /* Can't read CSD? */
                mmc_receive_block(mmc, csd, sizeof(csd));

                uint32_t read_bl_len = csd[5] & 0x0F;
                uint32_t c_size = (csd[8] >> 6) + (csd[7] << 3) +
                                ((csd[6] & 0x03) << 10);
                uint32_t c_size_mult = ((csd[10] & 0x80) >> 7) +
                                ((csd[9] & 0x03) << 1);
                uint32_t blocknr = (c_size + 1) << (c_size_mult + 2 + read_bl_len);
                (void)blocknr;
		printf("%d,%d,%d\n",c_size,c_size_mult,blocknr);
        }


	mmc_release(mmc);

	return 0;
}

