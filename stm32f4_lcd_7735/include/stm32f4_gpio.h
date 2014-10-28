#ifndef _STM32F_GPIO_
#define _STM32F_GPIO_



#define GP_PIN0    0
#define GP_PIN1    1
#define GP_PIN2    2
#define GP_PIN3    3
#define GP_PIN4    4
#define GP_PIN5    5
#define GP_PIN6    6
#define GP_PIN7    7
#define GP_PIN8    8
#define GP_PIN9    9
#define GP_PIN10   10
#define GP_PIN11   11
#define GP_PIN12   12
#define GP_PIN13   13
#define GP_PIN14   14
#define GP_PIN15   15


#define GP_MODE_INPUT      0x00
#define GP_MODE_OUTPUT     0x01
#define GP_MODE_ALTERNATE  0x02
#define GP_MODE_ANALOG     0x03

#define GP_SPEED_LOW       0x00
#define GP_SPEED_MEDIUM    0x01
#define GP_SPEED_FAST      0x02
#define GP_SPEED_HIGH      0x03

#define GP_TYPE_PUPD       0x00 //pullup pulldown
#define GP_TYPE_OD         0x01 //open drain

#define GP_PULL_NONE       0x00 //pull none
#define GP_PULL_UP         0x01 //pull up 
#define GP_PULL_DOWN       0x02 //pulldown


#define GP_AF0    0 // system
#define GP_AF1    1 // timer 1 and timer 2
#define GP_AF2    2 // tim 3,4,5 
#define GP_AF3    3 // tim 9,10,11
#define GP_AF4    4 // I2C 1,2,3
#define GP_AF5    5 // SPI 1,2,3
#define GP_AF6    6 // SPI 2,3,4,5 
#define GP_AF7    7 // SPI3, USART 1,2
#define GP_AF8    8 // USART 6
#define GP_AF9    9 // I2C 2,3
#define GP_AF10   10 
#define GP_AF11   11
#define GP_AF12   12 // SDIO
#define GP_AF13   13
#define GP_AF14   14
#define GP_AF15   15 // Event out






#endif//_STM32F_GPIO_
