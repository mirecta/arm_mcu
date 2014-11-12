#include <stm32f10x.h>
#include "stm32f1_util.h"

#include "stm32f10x_conf.h"

#include "fsmc_sram.h"
#include "lcd_dis24.h"




void LCD_GPIO_Init();

int  main(){
    initDelay();

    /* Enable the FSMC Clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
    LCD_GPIO_Init();
    
    /* Configure FSMC Bank1 NOR/PSRAM */
    FSMC_LCD_Init();
    LCD_Init();


    while (1)
    {					  
        LCD_test();	  	
        GPIO_SetBits(GPIOB, GPIO_Pin_5);
        Delay(100);					   
        GPIO_ResetBits(GPIOB, GPIO_Pin_5);	     //V6 	   
        Delay(100);									

    }




}

void LCD_GPIO_Init(){

  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC |
                         RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE , ENABLE); 	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				     //LED1
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);					 
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;		 //backlight
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;		 //LCD ??
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 ; 	 //LCD-RST
  GPIO_Init(GPIOE, &GPIO_InitStructure);  	
  
  //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 ; 
  //GPIO_Init(GPIOD, &GPIO_InitStructure); 	
  
/*-- GPIO Configuration ------------------------------------------------------*/  
  //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 |
                                GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 | 
                                GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* Set PE.07(D4), PE.08(D5), PE.09(D6), PE.10(D7), PE.11(D8), PE.12(D9), PE.13(D10),
     PE.14(D11), PE.15(D12) as alternate function push pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | 
                                GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
                                GPIO_Pin_15;
  GPIO_Init(GPIOE, &GPIO_InitStructure); 


   /* LCD Data(D0-D7) lines configuration */
  
  //GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_14 | GPIO_Pin_15 |GPIO_Pin_0 | GPIO_Pin_1 ;
  //GPIO_Init(GPIOD, &GPIO_InitStructure); 
  
  //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 ;
  //GPIO_Init(GPIOE, &GPIO_InitStructure);
  
  /* NOE and NWE configuration */  
  //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 |GPIO_Pin_4;
  //GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  /* NE1 configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; 
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  /* RS */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 ; 
  GPIO_Init(GPIOD, &GPIO_InitStructure); 
	
  /* RST */
  //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 ; 
  //GPIO_Init(GPIOE, &GPIO_InitStructure);  

  
  
  

  
  GPIO_SetBits(GPIOD, GPIO_Pin_7);			//CS=1 
  GPIO_SetBits(GPIOD, GPIO_Pin_14| GPIO_Pin_15 |GPIO_Pin_0 | GPIO_Pin_1);  	 
  GPIO_SetBits(GPIOE, GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10);   
  GPIO_ResetBits(GPIOE, GPIO_Pin_0);
  GPIO_ResetBits(GPIOE, GPIO_Pin_1);		//RESET=0
  GPIO_SetBits(GPIOD, GPIO_Pin_4);		    //RD=1
  GPIO_SetBits(GPIOD, GPIO_Pin_5);			//WR=1
  GPIO_SetBits(GPIOD, GPIO_Pin_13);			//LIGHT

  //GPIO_SetBits(GPIOD, GPIO_Pin_11);			//RS
 
 }

// ++++++++++++++++TFT reset
void lcd_rst(void){
	GPIO_ResetBits(GPIOE, GPIO_Pin_1);
    Delay(100);					   
    GPIO_SetBits(GPIOE, GPIO_Pin_1 );		 	 
	Delay(100);	
}



