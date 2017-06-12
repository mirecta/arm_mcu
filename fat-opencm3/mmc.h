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

/* MMC Card interface.
 */

#ifndef __MMC_H
#define __MMC_H

#include <stdint.h>

#if defined (__cplusplus)
extern "C" {
#endif

#include "openfat/blockdev.h"

static volatile uint32_t time_timeout;

struct mmc_port {
	struct block_device bldev;
	/* Physical hardware config */
	uint32_t spi;
	uint32_t cs_port;
	uint16_t cs_pin;
        uint8_t card_type;
	/* Any state information */
	/* ... to be added ... */
};

/* Card type flags (CardType) */
#define CT_MMC              0x01
#define CT_SD1              0x02
#define CT_SD2              0x04
#define CT_SDC              (CT_SD1|CT_SD2)
#define CT_BLOCK            0x08

/* MMC command mnemonics in SPI mode */
#define MMC_GO_IDLE_STATE 	0
#define MMC_SEND_OP_COND	1
#define MMC_SWITCH		6
#define MMC_SEND_EXT_CSD	8
#define MMC_SEND_CSD		9
#define MMC_SEND_CID		10
#define MMC_STOP_TRANSMISSION	12
#define MMC_SEND_STATUS		13
#define MMC_SET_BLOCKLEN	16
#define MMC_READ_SINGLE_BLOCK	17
#define MMC_READ_MULTIPLE_BLOCK	18
#define MMC_SET_BLOCK_COUNT	23
#define MMC_WRITE_BLOCK		24
#define MMC_WRITE_MULTIPLE_BLOCK	25
#define MMC_PROGRAM_CSD		27
#define MMC_SET_WRITE_PROT	28
#define MMC_CLR_WRITE_PROT	29
#define MMC_SEND_WRITE_PROT	30
#define MMC_ERASE_GROUP_START	35
#define MMC_ERASE_GROUP_END	36
#define MMC_ERASE		38
#define MMC_LOCK_UNLOCK		42
#define MMC_APP_CMD		55
#define MMC_GEN_CMD		56
#define MMC_READ_OCR		58
#define MMC_CRC_ON_OFF		59


/* Definitions for MMC/SDC command */
#define CMD0	(0x40+0)	/* GO_IDLE_STATE */
#define CMD1	(0x40+1)	/* SEND_OP_COND (MMC) */
#define ACMD41	(0xC0+41)	/* SEND_OP_COND (SDC) */
#define CMD8	(0x40+8)	/* SEND_IF_COND */
#define CMD9	(0x40+9)	/* SEND_CSD */
#define CMD10	(0x40+10)	/* SEND_CID */
#define CMD12	(0x40+12)	/* STOP_TRANSMISSION */
#define ACMD13	(0xC0+13)	/* SD_STATUS (SDC) */
#define CMD16	(0x40+16)	/* SET_BLOCKLEN */
#define CMD17	(0x40+17)	/* READ_SINGLE_BLOCK */
#define CMD18	(0x40+18)	/* READ_MULTIPLE_BLOCK */
#define CMD23	(0x40+23)	/* SET_BLOCK_COUNT (MMC) */
#define ACMD23	(0xC0+23)	/* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24	(0x40+24)	/* WRITE_BLOCK */
#define CMD25	(0x40+25)	/* WRITE_MULTIPLE_BLOCK */
#define CMD55	(0x40+55)	/* APP_CMD */
#define CMD58	(0x40+58)	/* READ_OCR */

int mmc_init(uint32_t spi, uint32_t cs_port, uint16_t cs_pin, struct mmc_port *mmc);

#if defined (__cplusplus)
}
#endif

#endif

