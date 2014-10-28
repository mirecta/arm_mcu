/**
  ******************************************************************************
  * @file    st7735.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    22-April-2014
  * @brief   This file includes the driver for ST7735 LCD mounted on the Adafruit
  *          1.8" TFT LCD shield (reference ID 802).
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "st7735.h"

/** @addtogroup BSP
  * @{
  */ 

/** @addtogroup Components
  * @{
  */ 

/** @addtogroup ST7735
  * @brief      This file provides a set of functions needed to drive the
  *             ST7735 LCD.
  * @{
  */

/** @defgroup ST7735_Private_TypesDefinitions
  * @{
  */ 

/**
  * @}
  */ 

/** @defgroup ST7735_Private_Defines
  * @{
  */

/**
  * @}
  */ 

/** @defgroup ST7735_Private_Macros
  * @{
  */

/**
  * @}
  */  

/** @defgroup ST7735_Private_Variables
  * @{
  */ 



/**
* @}
*/ 

/** @defgroup ST7735_Private_FunctionPrototypes
  * @{
  */

/**
* @}
*/ 

/** @defgroup ST7735_Private_Functions
  * @{
  */

/**
  * @brief  Initialize the ST7735 LCD Component.
  * @param  None
  * @retval None
  */
void st7735_Init(void)
{    
  /* Initialize ST7735 low level bus layer -----------------------------------*/
  LCD_IO_Init();
  LCD_IO_WriteReg(0x01);
  /* Out of sleep mode, 0 args, no delay */
  st7735_WriteReg(LCD_REG_17,0x00);
  
  //st7735_WriteReg(LCD_REG_17, 0x00); 
  /* Frame rate ctrl - normal mode, 3 args:Rate = fosc/(1x2+40) * (LINE+2C+2D)*/
  LCD_IO_WriteReg(LCD_REG_177);
  LCD_IO_WriteData(0x01);
  LCD_IO_WriteData(0x2C);
  LCD_IO_WriteData(0x2D);
  /* Frame rate control - idle mode, 3 args:Rate = fosc/(1x2+40) * (LINE+2C+2D) */    
  st7735_WriteReg(LCD_REG_178, 0x01);
  st7735_WriteReg(LCD_REG_178, 0x2C);
  st7735_WriteReg(LCD_REG_178, 0x2D);
  /* Frame rate ctrl - partial mode, 6 args: Dot inversion mode, Line inversion mode */ 
  st7735_WriteReg(LCD_REG_179, 0x01);
  st7735_WriteReg(LCD_REG_179, 0x2C);
  st7735_WriteReg(LCD_REG_179, 0x2D);
  st7735_WriteReg(LCD_REG_179, 0x01);
  st7735_WriteReg(LCD_REG_179, 0x2C);
  st7735_WriteReg(LCD_REG_179, 0x2D);
  /* Display inversion ctrl, 1 arg, no delay: No inversion */
  st7735_WriteReg(LCD_REG_180, 0x07);
  /* Power control, 3 args, no delay: -4.6V , AUTO mode */
  st7735_WriteReg(LCD_REG_192, 0xA2);
  st7735_WriteReg(LCD_REG_192, 0x02);
  st7735_WriteReg(LCD_REG_192, 0x84);
  /* Power control, 1 arg, no delay: VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD */
  st7735_WriteReg(LCD_REG_193, 0xC5);
  /* Power control, 2 args, no delay: Opamp current small, Boost frequency */ 
  st7735_WriteReg(LCD_REG_194, 0x0A);
  st7735_WriteReg(LCD_REG_194, 0x00);
  /* Power control, 2 args, no delay: BCLK/2, Opamp current small & Medium low */  
  st7735_WriteReg(LCD_REG_195, 0x8A);
  st7735_WriteReg(LCD_REG_195, 0x2A);
  /* Power control, 2 args, no delay */
  st7735_WriteReg(LCD_REG_196, 0x8A);
  st7735_WriteReg(LCD_REG_196, 0xEE);
  /* Power control, 1 arg, no delay */
  st7735_WriteReg(LCD_REG_197, 0x0E);
  /* Don't invert display, no args, no delay */
  LCD_IO_WriteReg(LCD_REG_32);
  /* Set color mode, 1 arg, no delay: 16-bit color */
  st7735_WriteReg(LCD_REG_58, 0x05);
  /* Magical unicorn dust, 16 args, no delay */
  st7735_WriteReg(LCD_REG_224, 0x02); 
  st7735_WriteReg(LCD_REG_224, 0x1c);  
  st7735_WriteReg(LCD_REG_224, 0x07); 
  st7735_WriteReg(LCD_REG_224, 0x12);
  st7735_WriteReg(LCD_REG_224, 0x37);  
  st7735_WriteReg(LCD_REG_224, 0x32);  
  st7735_WriteReg(LCD_REG_224, 0x29);  
  st7735_WriteReg(LCD_REG_224, 0x2d);
  st7735_WriteReg(LCD_REG_224, 0x29);  
  st7735_WriteReg(LCD_REG_224, 0x25);  
  st7735_WriteReg(LCD_REG_224, 0x2B);  
  st7735_WriteReg(LCD_REG_224, 0x39);  
  st7735_WriteReg(LCD_REG_224, 0x00);  
  st7735_WriteReg(LCD_REG_224, 0x01);  
  st7735_WriteReg(LCD_REG_224, 0x03);  
  st7735_WriteReg(LCD_REG_224, 0x10);
  /* Sparkles and rainbows, 16 args, no delay */
  st7735_WriteReg(LCD_REG_225, 0x03);
  st7735_WriteReg(LCD_REG_225, 0x1d);  
  st7735_WriteReg(LCD_REG_225, 0x07);  
  st7735_WriteReg(LCD_REG_225, 0x06);
  st7735_WriteReg(LCD_REG_225, 0x2E);  
  st7735_WriteReg(LCD_REG_225, 0x2C);  
  st7735_WriteReg(LCD_REG_225, 0x29);  
  st7735_WriteReg(LCD_REG_225, 0x2D);
  st7735_WriteReg(LCD_REG_225, 0x2E);  
  st7735_WriteReg(LCD_REG_225, 0x2E);  
  st7735_WriteReg(LCD_REG_225, 0x37);  
  st7735_WriteReg(LCD_REG_225, 0x3F);  
  st7735_WriteReg(LCD_REG_225, 0x00);  
  st7735_WriteReg(LCD_REG_225, 0x00);  
  st7735_WriteReg(LCD_REG_225, 0x02);  
  st7735_WriteReg(LCD_REG_225, 0x10);
  /* Normal display on, no args, no delay */
  st7735_WriteReg(LCD_REG_19, 0x00);
  /* Main screen turn on, no delay */
  st7735_WriteReg(LCD_REG_41, 0x00);
  st7735_ResetArea();
}

/**
  * @brief  Enables the Display.
  * @param  None
  * @retval None
  */
void st7735_DisplayOn(void)
{
  LCD_IO_WriteReg(LCD_REG_19);
  LCD_Delay(10);
  LCD_IO_WriteReg(LCD_REG_41);
  LCD_Delay(10);
  LCD_IO_WriteReg(LCD_REG_54);
  LCD_IO_WriteData(0x60);
}

/**
  * @brief  Disables the Display.
  * @param  None
  * @retval None
  */
void st7735_DisplayOff(void)
{
  LCD_IO_WriteReg(LCD_REG_19);
  LCD_Delay(10);
  LCD_IO_WriteReg(LCD_REG_40);
  LCD_Delay(10);
  LCD_IO_WriteReg(LCD_REG_54);
  LCD_IO_WriteData(0x60);
}

/**
  * @brief  Sets Cursor position.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @retval None
  */
void st7735_SetCursor(int16_t Xpos, int16_t Ypos)
{
  LCD_IO_WriteReg(LCD_REG_42);
  LCD_IO_WriteData16(Xpos);
  LCD_IO_WriteData16(Xpos);
  LCD_IO_WriteReg(LCD_REG_43); 
  LCD_IO_WriteData16(Ypos);
  LCD_IO_WriteData16(Ypos);
  LCD_IO_WriteReg(LCD_REG_44);
}

/**
  * @brief  Writes pixel.   
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @param  RGBCode: the RGB pixel color
  * @retval None
  */
void st7735_WritePixel(int16_t x, int16_t y, uint16_t color)
{
  
  /* Set Cursor */
  if(x < 0) return;
  if(y < 0) return;
  if(x >= ST7735_LCD_PIXEL_WIDTH) return;
  if(y >= ST7735_LCD_PIXEL_HEIGHT) return;
  st7735_SetCursor(x, y);
  
  LCD_IO_WriteData16(color);
}  


/**
  * @brief  Writes to the selected LCD register.
  * @param  LCDReg: Address of the selected register.
  * @param  LCDRegValue: value to write to the selected register.
  * @retval None
  */
void st7735_WriteReg(uint8_t LCDReg, uint8_t LCDRegValue)
{
  LCD_IO_WriteReg(LCDReg);
  
  LCD_IO_WriteData(LCDRegValue);
}


/**
  * @brief  Gets the LCD pixel Width.
  * @param  None
  * @retval The Lcd Pixel Width
  */
uint16_t st7735_GetLcdPixelWidth(void)
{
  return ST7735_LCD_PIXEL_WIDTH;
}

/**
  * @brief  Gets the LCD pixel Height.
  * @param  None
  * @retval The Lcd Pixel Height
  */
uint16_t st7735_GetLcdPixelHeight(void)
{                          
  return ST7735_LCD_PIXEL_HEIGHT;
}

void st7735_SetArea(int16_t x1, int16_t x2, int16_t y1, int16_t y2){
  
  if (x1 < 0) x1=0;
  if (y1 < 0) y1=0;
  if (x1 >= ST7735_LCD_PIXEL_WIDTH) x1 = ST7735_LCD_PIXEL_WIDTH - 1;
  if (x2 >= ST7735_LCD_PIXEL_WIDTH) x2 = ST7735_LCD_PIXEL_WIDTH - 1;
  if (y1 >= ST7735_LCD_PIXEL_HEIGHT) y1 = ST7735_LCD_PIXEL_HEIGHT - 1;
  if (y2 >= ST7735_LCD_PIXEL_HEIGHT) y2 = ST7735_LCD_PIXEL_HEIGHT - 1;
  
  LCD_IO_WriteReg(LCD_REG_42);
  LCD_IO_WriteData16(x1);
  LCD_IO_WriteData16(x2);
  LCD_IO_WriteReg(LCD_REG_43); 
  LCD_IO_WriteData16(y1);
  LCD_IO_WriteData16(y2);
  LCD_IO_WriteReg(LCD_REG_44);

}

void st7735_WritePixelRaw(uint16_t color){
 
  LCD_IO_WriteData16(color);
}

void st7735_ResetArea(void){
  LCD_IO_WriteReg(LCD_REG_42);
  LCD_IO_WriteData16(0);
  LCD_IO_WriteData16(ST7735_LCD_PIXEL_WIDTH-1);
  LCD_IO_WriteReg(LCD_REG_43); 
  LCD_IO_WriteData16(0);
  LCD_IO_WriteData16(ST7735_LCD_PIXEL_HEIGHT-1);
  LCD_IO_WriteReg(LCD_REG_44);
}

void st7735_Clear(uint16_t color){
 st7735_ResetArea();
 for(int i = 0; i < ST7735_LCD_PIXEL_WIDTH * ST7735_LCD_PIXEL_HEIGHT; ++i)
        LCD_IO_WriteData16(color);
}


/**
* @}
*/ 

/**
* @}
*/ 

/**
* @}
*/ 

/**
* @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
