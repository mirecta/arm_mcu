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
// https://github.com/Laurenceb/STM32-Logger/blob/master/Util/fat_fs/src/sd_spi_stm32.c
/* MMC Card interface implementation.
 */

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>
#include <string.h>
#include "diskio.h"


/* MMC/SD command */
#define CMD0	(0)			/* GO_IDLE_STATE */
#define CMD1	(1)			/* SEND_OP_COND (MMC) */
#define	ACMD41	(0x80+41)	/* SEND_OP_COND (SDC) */
#define CMD8	(8)			/* SEND_IF_COND */
#define CMD9	(9)			/* SEND_CSD */
#define CMD10	(10)		/* SEND_CID */
#define CMD12	(12)		/* STOP_TRANSMISSION */
#define ACMD13	(0x80+13)	/* SD_STATUS (SDC) */
#define CMD16	(16)		/* SET_BLOCKLEN */
#define CMD17	(17)		/* READ_SINGLE_BLOCK */
#define CMD18	(18)		/* READ_MULTIPLE_BLOCK */
#define CMD23	(23)		/* SET_BLOCK_COUNT (MMC) */
#define	ACMD23	(0x80+23)	/* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24	(24)		/* WRITE_BLOCK */
#define CMD25	(25)		/* WRITE_MULTIPLE_BLOCK */
#define CMD32	(32)		/* ERASE_ER_BLK_START */
#define CMD33	(33)		/* ERASE_ER_BLK_END */
#define CMD38	(38)		/* ERASE */
#define CMD55	(55)		/* APP_CMD */
#define CMD58	(58)		/* READ_OCR */


static struct mmc_port {
	/* Physical hardware config */
	uint32_t spi;
	uint32_t cs_port;
	uint16_t cs_pin;
    uint8_t card_type;
	/* Any state information */
	/* ... to be added ... */
} mmc;

static volatile
DSTATUS Stat = STA_NOINIT;	/* Physical drive status */

static volatile
UINT Timer1, Timer2;	/* 1kHz decrement timer stopped at zero (disk_timerproc()) */


static uint8_t spi_readwrite(uint32_t spi, uint8_t data)
{
    return spi_xfer(spi, data);

}

/*-----------------------------------------------------------------------*/
/* Wait for card ready                                                   */
/*-----------------------------------------------------------------------*/

static
int wait_ready (	/* 1:Ready, 0:Timeout */
	UINT wt			/* Timeout [ms] */
)
{
	BYTE d;


	Timer2 = wt;
	do {
		d = spi_readwrite(mmc.spi, 0xFF);
		/* This loop takes a time. Insert rot_rdq() here for multitask envilonment. */
	} while (d != 0xFF && Timer2);	/* Wait for card goes ready or timeout */

	return (d == 0xFF) ? 1 : 0;
}


static void mmc_release()
{
	gpio_set(mmc.cs_port, mmc.cs_pin);
	/* Must cycle clock 8 times after CS is released */
	spi_readwrite(mmc.spi, 0xFF);
}

static int mmc_select()
{
	gpio_clear(mmc.cs_port, mmc.cs_pin);
	/* Wait for not busy */
	if (wait_ready(500)) return 1;	/* Wait for card ready */

	mmc_release();
    return 0;	/* Timeout */

}

static void 
mmc_write_buffer(const uint8_t *buff, int len)
{
	while(len--)
		spi_readwrite(mmc.spi, *buff++);
}

static void 
mmc_read_buffer(uint8_t *buff, int len)
{
	while(len--)
		*buff++ = spi_readwrite(mmc.spi, 0xFF);
}

static int
mmc_receive_block(uint8_t *buff, int len)
{
	uint8_t token;


	Timer1 = 200;
	do {							/* Wait for DataStart token in timeout of 200ms */
		token = spi_readwrite(mmc.spi, 0xFF);
		/* This loop will take a time. Insert rot_rdq() here for multitask envilonment. */
	} while ((token == 0xFF) && Timer1);
	if(token != 0xFE) return 0;		/* Function fails if invalid DataStart token or timeout */

	mmc_read_buffer(buff, len);		/* Store trailing data to the buffer */
	
    spi_readwrite(mmc.spi, 0xFF);  	/* Discard CRC bytes */  
	spi_readwrite(mmc.spi, 0xFF);

	return 1;						/* Function succeeded */
}

static int
mmc_transmit_block(const uint8_t *buff, uint8_t  token)
{
    uint8_t res;
    /* wait for not busy */
	while(spi_readwrite(mmc.spi, 0xFF) != 0xFF);

	/* Send token */
	spi_readwrite(mmc.spi, token);

	/* Sent data frame */
	mmc_write_buffer(buff, 512);

	/* Send dummy CRC bytes */
	spi_readwrite(mmc.spi, 0xFF);
	spi_readwrite(mmc.spi, 0xFF);

	/* Return response code, zero on no error */
	res = spi_readwrite(mmc.spi, 0xFF);
    
    if ((res & 0x1F) != 0x05) return 0;
    
    return 1;
}

static uint8_t
mmc_command(uint8_t cmd, uint32_t arg)
{
	uint8_t buff[6];
	uint8_t ret;

	if (cmd & 0x80){
		cmd &= 0x7F;
		ret = mmc_command(CMD55, 0);
		if (ret > 1) return ret;
	}
		

	buff[0] = cmd | 0x40;
	/* argument is packed big-endian */
	buff[1] = (arg >> 24) & 0xFF;
	buff[2] = (arg >> 16) & 0xFF;
	buff[3] = (arg >> 8) & 0xFF;
	buff[4] = arg & 0xFF;
        buff[5] = 0x01;  /* Dummy CRC + Stop */
	if (cmd == CMD0) buff[5] = 0x95; /* Valid CRC for CMD0(0) */
	if (cmd == CMD8) buff[5] = 0x87; /* Valid CRC for CMD8(0x1AA) */


	mmc_select();
	mmc_write_buffer(buff, sizeof(buff));
	
	if (cmd == CMD12) spi_readwrite(mmc.spi, 0xFF);

        int n = 10;

	do { /* Wait for response byte */
		ret = spi_readwrite(mmc.spi, 0xFF);
	} while((ret & 0x80) && --n);

	return ret;
}

int mmc_init(uint32_t spi, uint32_t cs_port, uint16_t cs_pin)
{
	uint8_t n,ocr[4],cmd;

	/* Intialise structure */
	memset(&mmc, 0, sizeof(mmc));

	/* Block device methods */

	mmc.spi = spi;
	mmc.cs_port = cs_port;
	mmc.cs_pin = cs_pin;

    spi_reset(mmc.spi);
	/* Do hardware init */
	/* Peripheral clocks must already be enabled.  
	 * SPI pins must already be configured. */
	spi_init_master(mmc.spi, 
			SPI_CR1_BAUDRATE_FPCLK_DIV_4, 
			SPI_CR1_CPOL_CLK_TO_1_WHEN_IDLE, 
			SPI_CR1_CPHA_CLK_TRANSITION_2, 
			SPI_CR1_DFF_8BIT,
			SPI_CR1_MSBFIRST);
	/* Ignore the stupid NSS pin */
	spi_enable_software_slave_management(mmc.spi);
	spi_set_nss_high(mmc.spi);

	spi_enable(mmc.spi);

	/* SD nCS pin init */
	gpio_set_mode(mmc.cs_port, GPIO_MODE_OUTPUT_50_MHZ, 
			GPIO_CNF_OUTPUT_PUSHPULL, mmc.cs_pin);
    mmc_select();
	/* Do card init ... */
	for(int i = 0 ; i < 80; i++) spi_readwrite(mmc.spi, 0xff);
	
	if (mmc_command(CMD0, 0) == 1) {			/* Enter Idle state */
		Timer1 = 1000;						/* Initialization timeout of 1000 milliseconds */
		if (mmc_command(CMD8, 0x1AA) == 1) {	/* SDHC */
			for (n = 0; n < 4; n++) ocr[n] = spi_readwrite(mmc.spi, 0xff);		/* Get trailing return value of R7 response */
			if (ocr[2] == 0x01 && ocr[3] == 0xAA) {				/* The card can work at VDD range of 2.7-3.6V */
				while (Timer1 && mmc_command(ACMD41, 1UL << 30));	/* Wait for leaving idle state (ACMD41 with HCS bit) */
				if (Timer1 && mmc_command(CMD58, 0) == 0) {		/* Check CCS bit in the OCR */
					for (n = 0; n < 4; n++) ocr[n] = spi_readwrite(mmc.spi, 0xff);
					mmc.card_type = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;
				}
			}
		} else {							/* SDSC or MMC */
			if (mmc_command(ACMD41, 0) <= 1) 	{
				mmc.card_type = CT_SD1; cmd = ACMD41;	/* SDSC */
			} else {
				mmc.card_type = CT_MMC; cmd = CMD1;	/* MMC */
			}
			while (Timer1 && mmc_command(cmd, 0));			/* Wait for leaving idle state */
			if (!Timer1 || mmc_command(CMD16, 512) != 0)	/* Set R/W block length to 512 */
				mmc.card_type = 0;
		}
	}
	


	mmc_release();
	if (mmc.card_type) {			/* OK */
		Stat &= ~STA_NOINIT;	/* Clear STA_NOINIT flag */
	} else {			/* Failed */
		Stat = STA_NOINIT;
	}

	return Stat;

}

/*--------------------------------------------------------------------------

   Public Functions

---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/* Initialize disk drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE drv		/* Physical drive number (0) */
)
{


	if (drv) return STA_NOINIT;			/* Supports only drive 0 */

	if (Stat & STA_NODISK) return Stat;	/* Is card existing in the soket? */
    return mmc_init(SPI2,GPIOB,GPIO12);
}

/*-----------------------------------------------------------------------*/
/* Get disk status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE drv		/* Physical drive number (0) */
)
{
	if (drv) return STA_NOINIT;		/* Supports only drive 0 */

	return Stat;	/* Return disk status */
}

/*-----------------------------------------------------------------------*/
/* Read sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE drv,		/* Physical drive number (0) */
	BYTE *buff,		/* Pointer to the data buffer to store read data */
	DWORD sector,	/* Start sector number (LBA) */
	UINT count		/* Number of sectors to read (1..128) */
)
{
	if (drv || !count) return RES_PARERR;		/* Check parameter */
	if (Stat & STA_NOINIT) return RES_NOTRDY;	/* Check if drive is ready */


	if (!(mmc.card_type & CT_BLOCK)) sector *= 512;	/* Convert to byte address if needed */
	mmc_select();

	if (count == 1) {	/* Single block read */
		if (mmc_command(CMD17, sector) == 0)	{ /* READ_SINGLE_BLOCK */
			if (mmc_receive_block(buff, 512)) {
				count = 0;
			}
		}
	}else {				/* Multiple block read */
		if (mmc_command(CMD18, sector) == 0) {	/* READ_MULTIPLE_BLOCK */
			do {
				if (!mmc_receive_block(buff, 512)) {
					break;
				}
				buff += 512;
			} while (--count);
			mmc_command(CMD12, 0);		/* STOP_TRANSMISSION */
		}
        }
	mmc_release();

    return count ? RES_ERROR : RES_OK;	/* Return result */

}

/*-----------------------------------------------------------------------*/
/* Write sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
	BYTE drv,			/* Physical drive number (0) */
	const BYTE *buff,	/* Ponter to the data to write */
	DWORD sector,		/* Start sector number (LBA) */
	UINT count			/* Number of sectors to write (1..128) */
)
{
	if (drv || !count) return RES_PARERR;		/* Check parameter */
	if (Stat & STA_NOINIT) return RES_NOTRDY;	/* Check drive status */
	if (Stat & STA_PROTECT) return RES_WRPRT;	/* Check write protect */

	if (!(mmc.card_type & CT_BLOCK)) sector *= 512;	/* LBA ==> BA conversion (byte addressing cards) */

	mmc_select();

	if (count == 1) {	/* Single sector write */
		if ((mmc_command(CMD24, sector) == 0)	/* WRITE_BLOCK */
			&& mmc_transmit_block(buff, 0xFE)) {
			count = 0;
		}
	}
	else {				/* Multiple sector write */
		if (mmc.card_type & CT_SDC) mmc_command(ACMD23, count);	/* Predefine number of sectors */
		if (mmc_command(CMD25, sector) == 0) {	/* WRITE_MULTIPLE_BLOCK */
			do {
				if (!mmc_transmit_block(buff, 0xFC)) break;
				buff += 512;
			} while (--count);
			if (!mmc_transmit_block(0, 0xFD)) count = 1;	/* STOP_TRAN token */
		}
	}
    mmc_release();

	return count ? RES_ERROR : RES_OK;	/* Return result */
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous drive controls other than data read/write               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive number (0) */
	BYTE cmd,		/* Control command code */
	void *buff		/* Pointer to the conrtol data */
)
{
	DRESULT res;
	BYTE n, csd[16];
	DWORD *dp, st, ed, csize;


	if (drv) return RES_PARERR;					/* Check parameter */
	if (Stat & STA_NOINIT) return RES_NOTRDY;	/* Check if drive is ready */

    mmc_select();
	res = RES_ERROR;

	switch (cmd) {
	case CTRL_SYNC :		/* Wait for end of internal write process of the drive */
		if (mmc_select()) res = RES_OK;
		break;

	case GET_SECTOR_COUNT :	/* Get drive capacity in unit of sector (DWORD) */
		if ((mmc_command(CMD9, 0) == 0) && mmc_receive_block(csd, 16)) {
			if ((csd[0] >> 6) == 1) {	/* SDC ver 2.00 */
				csize = csd[9] + ((WORD)csd[8] << 8) + ((DWORD)(csd[7] & 63) << 16) + 1;
				*(DWORD*)buff = csize << 10;
			} else {					/* SDC ver 1.XX or MMC ver 3 */
				n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
				csize = (csd[8] >> 6) + ((WORD)csd[7] << 2) + ((WORD)(csd[6] & 3) << 10) + 1;
				*(DWORD*)buff = csize << (n - 9);
			}
			res = RES_OK;
		}
		break;

	case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (DWORD) */
		if (mmc.card_type & CT_SD2) {	/* SDC ver 2.00 */
			if (mmc_command(ACMD13, 0) == 0) {	/* Read SD status */
				spi_readwrite(mmc.spi, 0xFF);
				if (mmc_receive_block(csd, 16)) {				/* Read partial block */
					for (n = 64 - 16; n; n--) spi_readwrite(mmc.spi, 0xFF);	/* Purge trailing data */
					*(DWORD*)buff = 16UL << (csd[10] >> 4);
					res = RES_OK;
				}
			}
		} else {					/* SDC ver 1.XX or MMC */
			if ((send_cmd(CMD9, 0) == 0) && mmc_receive_block(csd, 16)) {	/* Read CSD */
				if (mmc.card_type& CT_SD1) {	/* SDC ver 1.XX */
					*(DWORD*)buff = (((csd[10] & 63) << 1) + ((WORD)(csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
				} else {					/* MMC */
					*(DWORD*)buff = ((WORD)((csd[10] & 124) >> 2) + 1) * (((csd[11] & 3) << 3) + ((csd[11] & 224) >> 5) + 1);
				}
				res = RES_OK;
			}
		}
		break;

	case CTRL_TRIM :	/* Erase a block of sectors (used when _USE_ERASE == 1) */
		if (!(mmc.card_type & CT_SDC)) break;				/* Check if the card is SDC */
		if (disk_ioctl(drv, MMC_GET_CSD, csd)) break;	/* Get CSD */
		if (!(csd[0] >> 6) && !(csd[10] & 0x40)) break;	/* Check if sector erase can be applied to the card */
		dp = buff; st = dp[0]; ed = dp[1];				/* Load sector block */
		if (!(mmc.card_type & CT_BLOCK)) {
			st *= 512; ed *= 512;
		}
		if (mmc_command(CMD32, st) == 0 && mmc_command(CMD33, ed) == 0 && mmc_command(CMD38, 0) == 0 && wait_ready(30000)) {	/* Erase sector block */
			res = RES_OK;	/* FatFs does not check result of this command */
		}
		break;

	default:
		res = RES_PARERR;
	}

	mmc_release();

	return res;
}
#endif







/*-----------------------------------------------------------------------*/
/* Device timer function                                                 */
/*-----------------------------------------------------------------------*/
/* This function must be called from timer interrupt routine in period
/  of 1 ms to generate card control timing.
*/

void disk_timerproc (void)
{
	WORD n;
	BYTE s;


	n = Timer1;						/* 1kHz decrement timer stopped at 0 */
	if (n) Timer1 = --n;
	n = Timer2;
	if (n) Timer2 = --n;

	s = Stat;
	if (false) {	/* Write protected */
		s |= STA_PROTECT;
	} else {		/* Write enabled */
		s &= ~STA_PROTECT;
	}
	if (true) {	/* Card is in socket */
		s &= ~STA_NODISK;
	} else {		/* Socket empty */
		s |= (STA_NODISK | STA_NOINIT);
	}
	Stat = s;
}

