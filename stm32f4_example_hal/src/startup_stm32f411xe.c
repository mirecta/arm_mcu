 
#define __weak __attribute__((weak))
#define __weak_alias_Default_Handler(f) void f(void)  __attribute__((weak, alias ("Default_Handler")))

extern unsigned long _sidata; //start data in flash
extern unsigned long _sdata;  //start data in RAM
extern unsigned long _edata;  //end data in RAM
extern unsigned long _sbss;  //start bss sector
extern unsigned long _ebss; //end bss sector
extern unsigned long _estack; //stack end 


int main(void);

__weak void SystemInit(void){
//override this func to setup PLL clock which deppends on your MCU
}




void Reset_Handler(void)
{
    unsigned long *source;
    unsigned long *destination;
    // Copying data from Flash to RAM (init global variables vit their valuses)
    source = &_sidata;
    for (destination = &_sdata; destination < &_edata;)
    {
        *(destination++) = *(source++);
    }
    // default zero to undefined global variables
    for (destination = &_sbss; destination < &_ebss;)
    {
        *(destination++) = 0;
    }
    // setup clock
    SystemInit();
    // starting main program
    main();
}


//default interrupt handler
void Default_Handler(void){
   while(1);
}



//aliases interrupts handlers to default , if u want use it u must override it 
   __weak_alias_Default_Handler( NMI_Handler);
  
   __weak_alias_Default_Handler( HardFault_Handler);
  
   __weak_alias_Default_Handler( MemManage_Handler);
  
   __weak_alias_Default_Handler( BusFault_Handler);

   __weak_alias_Default_Handler( UsageFault_Handler);

   __weak_alias_Default_Handler( SVC_Handler);

   __weak_alias_Default_Handler( DebugMon_Handler);

   __weak_alias_Default_Handler( PendSV_Handler);

   __weak_alias_Default_Handler( SysTick_Handler);
  
   __weak_alias_Default_Handler( WWDG_IRQHandler);                 
                  
   __weak_alias_Default_Handler( PVD_IRQHandler);  
               
   __weak_alias_Default_Handler( TAMP_STAMP_IRQHandler);
            
   __weak_alias_Default_Handler( RTC_WKUP_IRQHandler);              
            
   __weak_alias_Default_Handler( FLASH_IRQHandler);    
                  
   __weak_alias_Default_Handler( RCC_IRQHandler);  
                  
   __weak_alias_Default_Handler( EXTI0_IRQHandler);         
                  
   __weak_alias_Default_Handler( EXTI1_IRQHandler);       
                     
   __weak_alias_Default_Handler( EXTI2_IRQHandler);       
                 
   __weak_alias_Default_Handler( EXTI3_IRQHandler);       
                        
   __weak_alias_Default_Handler( EXTI4_IRQHandler);       
                  
   __weak_alias_Default_Handler( DMA1_Stream0_IRQHandler);
         
   __weak_alias_Default_Handler( DMA1_Stream1_IRQHandler);             
                  
   __weak_alias_Default_Handler( DMA1_Stream2_IRQHandler);             
                  
   __weak_alias_Default_Handler( DMA1_Stream3_IRQHandler);               
                 
   __weak_alias_Default_Handler( DMA1_Stream4_IRQHandler);            
                  
   __weak_alias_Default_Handler( DMA1_Stream5_IRQHandler);             
                  
   __weak_alias_Default_Handler( DMA1_Stream6_IRQHandler);             
                  
   __weak_alias_Default_Handler( ADC_IRQHandler);
            
   __weak_alias_Default_Handler( EXTI9_5_IRQHandler);   
            
   __weak_alias_Default_Handler( TIM1_BRK_TIM9_IRQHandler);            
            
   __weak_alias_Default_Handler( TIM1_UP_TIM10_IRQHandler);          
      
   __weak_alias_Default_Handler( TIM1_TRG_COM_TIM11_IRQHandler);      
      
   __weak_alias_Default_Handler( TIM1_CC_IRQHandler);
                  
   __weak_alias_Default_Handler( TIM2_IRQHandler);            
                  
   __weak_alias_Default_Handler( TIM3_IRQHandler);          
                  
   __weak_alias_Default_Handler( TIM4_IRQHandler);          
                  
   __weak_alias_Default_Handler( I2C1_EV_IRQHandler);   
                     
   __weak_alias_Default_Handler( I2C1_ER_IRQHandler); 
                     
   __weak_alias_Default_Handler( I2C2_EV_IRQHandler); 
                  
   __weak_alias_Default_Handler( I2C2_ER_IRQHandler); 
                           
   __weak_alias_Default_Handler( SPI1_IRQHandler);       
                        
   __weak_alias_Default_Handler( SPI2_IRQHandler);          
                  
   __weak_alias_Default_Handler( USART1_IRQHandler);      
                     
   __weak_alias_Default_Handler( USART2_IRQHandler);    
                                  
   __weak_alias_Default_Handler( EXTI15_10_IRQHandler);               
               
   __weak_alias_Default_Handler( RTC_Alarm_IRQHandler);             
            
   __weak_alias_Default_Handler( OTG_FS_WKUP_IRQHandler);         
            
   __weak_alias_Default_Handler( DMA1_Stream7_IRQHandler);              
                     
   __weak_alias_Default_Handler( SDIO_IRQHandler);            
                     
   __weak_alias_Default_Handler( TIM5_IRQHandler);          
                     
   __weak_alias_Default_Handler( SPI3_IRQHandler);          
                     
   __weak_alias_Default_Handler( DMA2_Stream0_IRQHandler);               
               
   __weak_alias_Default_Handler( DMA2_Stream1_IRQHandler);             
                  
   __weak_alias_Default_Handler( DMA2_Stream2_IRQHandler);             
            
   __weak_alias_Default_Handler( DMA2_Stream3_IRQHandler);             
            
   __weak_alias_Default_Handler( DMA2_Stream4_IRQHandler);             
            
   __weak_alias_Default_Handler( OTG_FS_IRQHandler);
                     
   __weak_alias_Default_Handler( DMA2_Stream5_IRQHandler);               
                  
   __weak_alias_Default_Handler( DMA2_Stream6_IRQHandler);             
                  
   __weak_alias_Default_Handler( DMA2_Stream7_IRQHandler);             
                  
   __weak_alias_Default_Handler( USART6_IRQHandler);
                        
   __weak_alias_Default_Handler( I2C3_EV_IRQHandler);  
                        
   __weak_alias_Default_Handler( I2C3_ER_IRQHandler); 
                        
   __weak_alias_Default_Handler( FPU_IRQHandler);            

   __weak_alias_Default_Handler( SPI4_IRQHandler);                 

   __weak_alias_Default_Handler( SPI5_IRQHandler);                




//interrupt vector table it depends on your MCU
__attribute__ ((section(".isr_vector")))
void (* const table_interrupt_vector[])(void) =
{
    (void *) &_estack, // 0 - stack
    Reset_Handler, // 1
    NMI_Handler,
    HardFault_Handler,
    MemManage_Handler,
    BusFault_Handler,
    UsageFault_Handler,
    0,
    0,
    0,
    0,
    SVC_Handler,
    DebugMon_Handler,
    0,
    PendSV_Handler,
    SysTick_Handler,
  
  /* External Interrupts */
       WWDG_IRQHandler,                  /* Window WatchDog              */                                        
       PVD_IRQHandler,                   /* PVD through EXTI Line detection */                        
       TAMP_STAMP_IRQHandler,            /* Tamper and TimeStamps through the EXTI line */            
       RTC_WKUP_IRQHandler,              /* RTC Wakeup through the EXTI line */                      
       FLASH_IRQHandler,                 /* FLASH                        */                                          
       RCC_IRQHandler,                   /* RCC                          */                                            
       EXTI0_IRQHandler,                 /* EXTI Line0                   */                        
       EXTI1_IRQHandler,                 /* EXTI Line1                   */                          
       EXTI2_IRQHandler,                 /* EXTI Line2                   */                          
       EXTI3_IRQHandler,                 /* EXTI Line3                   */                          
       EXTI4_IRQHandler,                 /* EXTI Line4                   */                          
       DMA1_Stream0_IRQHandler,          /* DMA1 Stream 0                */                  
       DMA1_Stream1_IRQHandler,          /* DMA1 Stream 1                */                   
       DMA1_Stream2_IRQHandler,          /* DMA1 Stream 2                */                   
       DMA1_Stream3_IRQHandler,          /* DMA1 Stream 3                */                   
       DMA1_Stream4_IRQHandler,          /* DMA1 Stream 4                */                   
       DMA1_Stream5_IRQHandler,          /* DMA1 Stream 5                */                   
       DMA1_Stream6_IRQHandler,          /* DMA1 Stream 6                */                   
       ADC_IRQHandler,                   /* ADC1, ADC2 and ADC3s         */                   
       0,              				  /* Reserved                      */                         
       0,             					  /* Reserved                     */                          
       0,                                /* Reserved                     */                          
       0,                                /* Reserved                     */                          
       EXTI9_5_IRQHandler,               /* External Line[9:5]s          */                          
       TIM1_BRK_TIM9_IRQHandler,         /* TIM1 Break and TIM9          */         
       TIM1_UP_TIM10_IRQHandler,         /* TIM1 Update and TIM10        */         
       TIM1_TRG_COM_TIM11_IRQHandler,    /* TIM1 Trigger and Commutation and TIM11 */
       TIM1_CC_IRQHandler,               /* TIM1 Capture Compare         */                          
       TIM2_IRQHandler,                  /* TIM2                         */                   
       TIM3_IRQHandler,                  /* TIM3                         */                   
       TIM4_IRQHandler,                  /* TIM4                         */                   
       I2C1_EV_IRQHandler,               /* I2C1 Event                   */                          
       I2C1_ER_IRQHandler,               /* I2C1 Error                   */                          
       I2C2_EV_IRQHandler,               /* I2C2 Event                   */                          
       I2C2_ER_IRQHandler,               /* I2C2 Error                   */                            
       SPI1_IRQHandler,                  /* SPI1                         */                   
       SPI2_IRQHandler,                  /* SPI2                         */                   
       USART1_IRQHandler,                /* USART1                       */                   
       USART2_IRQHandler,                /* USART2                       */                   
       0,               		    	  /* Reserved                       */                   
       EXTI15_10_IRQHandler,              /* External Line[15:10]s        */                          
       RTC_Alarm_IRQHandler,              /* RTC Alarm (A and B) through EXTI Line */                 
       OTG_FS_WKUP_IRQHandler,            /* USB OTG FS Wakeup through EXTI line */                       
       0,                                 /* Reserved     				  */         
       0,                                 /* Reserved       			  */         
       0,                                 /* Reserved 					  */
       0,                                 /* Reserved                     */                          
       DMA1_Stream7_IRQHandler,           /* DMA1 Stream7                 */                          
       0,                                 /* Reserved                     */                   
       SDIO_IRQHandler,                   /* SDIO                         */                   
       TIM5_IRQHandler,                   /* TIM5                         */                   
       SPI3_IRQHandler,                   /* SPI3                         */                   
       0,                                 /* Reserved                     */                   
       0,                                 /* Reserved                     */                   
       0,                                 /* Reserved                     */                   
       0,                                 /* Reserved                     */
       DMA2_Stream0_IRQHandler,           /* DMA2 Stream 0                */                   
       DMA2_Stream1_IRQHandler,           /* DMA2 Stream 1                */                   
       DMA2_Stream2_IRQHandler,           /* DMA2 Stream 2                */                   
       DMA2_Stream3_IRQHandler,           /* DMA2 Stream 3                */                   
       DMA2_Stream4_IRQHandler,           /* DMA2 Stream 4                */                   
       0,                    			  /* Reserved                     */                   
       0,              					  /* Reserved                     */                     
       0,              					  /* Reserved                     */                          
       0,             					  /* Reserved                     */                          
       0,              					  /* Reserved                     */                          
       0,              					  /* Reserved                     */                          
       OTG_FS_IRQHandler,                 /* USB OTG FS                   */                   
       DMA2_Stream5_IRQHandler,           /* DMA2 Stream 5                */                   
       DMA2_Stream6_IRQHandler,           /* DMA2 Stream 6                */                   
       DMA2_Stream7_IRQHandler,           /* DMA2 Stream 7                */                   
       USART6_IRQHandler,                 /* USART6                       */                    
       I2C3_EV_IRQHandler,                /* I2C3 event                   */                          
       I2C3_ER_IRQHandler,               /* I2C3 error                   */                          
       0,                                 /* Reserved                     */                   
       0,                                 /* Reserved                     */                   
       0,                                 /* Reserved                     */                         
       0,                                 /* Reserved                     */                   
       0,                                 /* Reserved                     */                   
       0,                                 /* Reserved                     */                   
       0,                                 /* Reserved                     */
       FPU_IRQHandler,                    /* FPU                          */
       0,                                 /* Reserved                     */                   
       0,                                 /* Reserved                     */
       SPI4_IRQHandler,                   /* SPI4                         */
       SPI5_IRQHandler                    /* SPI5                         */  

};



