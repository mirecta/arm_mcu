#include <stdint.h>
#ifndef _SPI_BOARD_H_
#define _SPI_BOARD_H_




void spiBoardInit(void);

void  spiBoardLcdData16(uint16_t data);

uint8_t spiBoardSendRecv(uint8_t data);

void  spiBoardLcdData(uint8_t data);

void  spiBoardLcdCmd(uint8_t cmd);




#endif //_SPI_BOARD_H_





