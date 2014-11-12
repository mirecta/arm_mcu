#ifndef _LCD_DISP24_H_
#define _LCD_DISP24_H_




void LCD_Init(void);
void LCD_WR_REG(unsigned int index);
void LCD_WR_CMD(unsigned int index,unsigned int val);

void LCD_WR_Data(unsigned int val);
void LCD_WR_Data_8(unsigned int val);
void LCD_test(void);
void LCD_clear(unsigned int p);
void ini(void);
unsigned int LCD_RD_data(void);
void lcd_rst(void);

void lcd_wr_zf(unsigned int a, unsigned int b, unsigned int a1,unsigned int b1, unsigned int d,unsigned int e, unsigned char g, const unsigned char *f); 
void lcd_wr_pixel(unsigned int a, unsigned int b, unsigned int e) ;
const unsigned char *num_pub(unsigned int a);


#endif //_LCD_DISP24_H_
