

#include <stm32f10x.h>


#define __weak __attribute__((weak))
#define __weak_alias_Default_Handler(f) void f(void)  __attribute__((weak, alias ("Default_Handler")))

extern unsigned long _sidata; //start data in flash
extern unsigned long _sdata;  //start data in RAM
extern unsigned long _edata;  //end data in RAM
extern unsigned long _sbss;  //start bss sector
extern unsigned long _ebss; //end bss sector
extern unsigned long _estack; //stack end 


__weak void SystemInit(void){
//override this func to setup PLL clock which deppends on your MCU
}

void  main();


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
__weak_alias_Default_Handler( NMI_Handler );
__weak_alias_Default_Handler( HardFault_Handler );
__weak_alias_Default_Handler( MemManage_Handler );
__weak_alias_Default_Handler( BusFault_Handler );
__weak_alias_Default_Handler( UsageFault_Handler );
__weak_alias_Default_Handler( SVC_Handler );
__weak_alias_Default_Handler( DebugMon_Handler );
__weak_alias_Default_Handler( PendSV_Handler );
__weak_alias_Default_Handler( SysTick_Handler );
__weak_alias_Default_Handler( WWDG_IRQHandler );
__weak_alias_Default_Handler( PVD_IRQHandler );
__weak_alias_Default_Handler( TAMPER_IRQHandler );
__weak_alias_Default_Handler( RTC_IRQHandler );
__weak_alias_Default_Handler( FLASH_IRQHandler );
__weak_alias_Default_Handler( RCC_IRQHandler );
__weak_alias_Default_Handler( EXTI0_IRQHandler );
__weak_alias_Default_Handler( EXTI1_IRQHandler );
__weak_alias_Default_Handler( EXTI2_IRQHandler );
__weak_alias_Default_Handler( EXTI3_IRQHandler );
__weak_alias_Default_Handler( EXTI4_IRQHandler );
__weak_alias_Default_Handler( DMA1_Channel1_IRQHandler );
__weak_alias_Default_Handler( DMA1_Channel2_IRQHandler );
__weak_alias_Default_Handler( DMA1_Channel3_IRQHandler );
__weak_alias_Default_Handler( DMA1_Channel4_IRQHandler );
__weak_alias_Default_Handler( DMA1_Channel5_IRQHandler );
__weak_alias_Default_Handler( DMA1_Channel6_IRQHandler );
__weak_alias_Default_Handler( DMA1_Channel7_IRQHandler );
__weak_alias_Default_Handler( ADC1_2_IRQHandler );
__weak_alias_Default_Handler( USB_HP_CAN1_TX_IRQHandler );
__weak_alias_Default_Handler( USB_LP_CAN1_RX0_IRQHandler );
__weak_alias_Default_Handler( CAN1_RX1_IRQHandler );
__weak_alias_Default_Handler( CAN1_SCE_IRQHandler );
__weak_alias_Default_Handler( EXTI9_5_IRQHandler );
__weak_alias_Default_Handler( TIM1_BRK_IRQHandler );
__weak_alias_Default_Handler( TIM1_UP_IRQHandler );
__weak_alias_Default_Handler( TIM1_TRG_COM_IRQHandler );
__weak_alias_Default_Handler( TIM1_CC_IRQHandler );
__weak_alias_Default_Handler( TIM2_IRQHandler );
__weak_alias_Default_Handler( TIM3_IRQHandler );
__weak_alias_Default_Handler( TIM4_IRQHandler );
__weak_alias_Default_Handler( I2C1_EV_IRQHandler );
__weak_alias_Default_Handler( I2C1_ER_IRQHandler );
__weak_alias_Default_Handler( I2C2_EV_IRQHandler );
__weak_alias_Default_Handler( I2C2_ER_IRQHandler );
__weak_alias_Default_Handler( SPI1_IRQHandler );
__weak_alias_Default_Handler( SPI2_IRQHandler );
__weak_alias_Default_Handler( USART1_IRQHandler );
__weak_alias_Default_Handler( USART2_IRQHandler );
__weak_alias_Default_Handler( USART3_IRQHandler );
__weak_alias_Default_Handler( EXTI15_10_IRQHandler );
__weak_alias_Default_Handler( RTCAlarm_IRQHandler );
__weak_alias_Default_Handler( USBWakeUp_IRQHandler );
__weak_alias_Default_Handler( TIM8_BRK_IRQHandler );
__weak_alias_Default_Handler( TIM8_UP_IRQHandler );
__weak_alias_Default_Handler( TIM8_TRG_COM_IRQHandler );
__weak_alias_Default_Handler( TIM8_CC_IRQHandler );
__weak_alias_Default_Handler( ADC3_IRQHandler );
__weak_alias_Default_Handler( FSMC_IRQHandler );
__weak_alias_Default_Handler( SDIO_IRQHandler );
__weak_alias_Default_Handler( TIM5_IRQHandler );
__weak_alias_Default_Handler( SPI3_IRQHandler );
__weak_alias_Default_Handler( UART4_IRQHandler );
__weak_alias_Default_Handler( UART5_IRQHandler );
__weak_alias_Default_Handler( TIM6_IRQHandler );
__weak_alias_Default_Handler( TIM7_IRQHandler );
__weak_alias_Default_Handler( DMA2_Channel1_IRQHandler );
__weak_alias_Default_Handler( DMA2_Channel2_IRQHandler );
__weak_alias_Default_Handler( DMA2_Channel3_IRQHandler );
__weak_alias_Default_Handler( DMA2_Channel4_5_IRQHandler );

//interrupt vector table it depends on your MCU
__attribute__ ((section(".isr_vector")))
void (* const table_interrupt_vector[])(void) =
{
    (void *) &_estack, // 0 - stack
      Reset_Handler , 		 // -15
      NMI_Handler , 		 // -14
      HardFault_Handler , 		 // -13
      MemManage_Handler , 		 // -12
      BusFault_Handler , 		 // -11
      UsageFault_Handler , 		 // -10
      0 , 		 // -9
      0 , 		 // -8
      0 , 		 // -7
      0 , 		 // -6
      SVC_Handler , 		 // -5
      DebugMon_Handler , 		 // -4
      0 , 		 // -3
      PendSV_Handler , 		 // -2
      SysTick_Handler , 		 // -1
      WWDG_IRQHandler , 		 // 0
      PVD_IRQHandler , 		 // 1
      TAMPER_IRQHandler , 		 // 2
      RTC_IRQHandler , 		 // 3
      FLASH_IRQHandler , 		 // 4
      RCC_IRQHandler , 		 // 5
      EXTI0_IRQHandler , 		 // 6
      EXTI1_IRQHandler , 		 // 7
      EXTI2_IRQHandler , 		 // 8
      EXTI3_IRQHandler , 		 // 9
      EXTI4_IRQHandler , 		 // 10
      DMA1_Channel1_IRQHandler , 		 // 11
      DMA1_Channel2_IRQHandler , 		 // 12
      DMA1_Channel3_IRQHandler , 		 // 13
      DMA1_Channel4_IRQHandler , 		 // 14
      DMA1_Channel5_IRQHandler , 		 // 15
      DMA1_Channel6_IRQHandler , 		 // 16
      DMA1_Channel7_IRQHandler , 		 // 17
      ADC1_2_IRQHandler , 		 // 18
      USB_HP_CAN1_TX_IRQHandler , 		 // 19
      USB_LP_CAN1_RX0_IRQHandler , 		 // 20
      CAN1_RX1_IRQHandler , 		 // 21
      CAN1_SCE_IRQHandler , 		 // 22
      EXTI9_5_IRQHandler , 		 // 23
      TIM1_BRK_IRQHandler , 		 // 24
      TIM1_UP_IRQHandler , 		 // 25
      TIM1_TRG_COM_IRQHandler , 		 // 26
      TIM1_CC_IRQHandler , 		 // 27
      TIM2_IRQHandler , 		 // 28
      TIM3_IRQHandler , 		 // 29
      TIM4_IRQHandler , 		 // 30
      I2C1_EV_IRQHandler , 		 // 31
      I2C1_ER_IRQHandler , 		 // 32
      I2C2_EV_IRQHandler , 		 // 33
      I2C2_ER_IRQHandler , 		 // 34
      SPI1_IRQHandler , 		 // 35
      SPI2_IRQHandler , 		 // 36
      USART1_IRQHandler , 		 // 37
      USART2_IRQHandler , 		 // 38
      USART3_IRQHandler , 		 // 39
      EXTI15_10_IRQHandler , 		 // 40
      RTCAlarm_IRQHandler , 		 // 41
      USBWakeUp_IRQHandler , 		 // 42
      TIM8_BRK_IRQHandler , 		 // 43
      TIM8_UP_IRQHandler , 		 // 44
      TIM8_TRG_COM_IRQHandler , 		 // 45
      TIM8_CC_IRQHandler , 		 // 46
      ADC3_IRQHandler , 		 // 47
      FSMC_IRQHandler , 		 // 48
      SDIO_IRQHandler , 		 // 49
      TIM5_IRQHandler , 		 // 50
      SPI3_IRQHandler , 		 // 51
      UART4_IRQHandler , 		 // 52
      UART5_IRQHandler , 		 // 53
      TIM6_IRQHandler , 		 // 54
      TIM7_IRQHandler , 		 // 55
      DMA2_Channel1_IRQHandler , 		 // 56
      DMA2_Channel2_IRQHandler , 		 // 57
      DMA2_Channel3_IRQHandler , 		 // 58
      DMA2_Channel4_5_IRQHandler , 		 // 59
      0 , 		 // 60
      0 , 		 // 61
      0 , 		 // 62
      0 , 		 // 63
      0 , 		 // 64
      0 , 		 // 65
      0 , 		 // 66
      0 , 		 // 67
      0 , 		 // 68
      0 , 		 // 69
      0 , 		 // 70
      0 , 		 // 71
      0 , 		 // 72
      0 , 		 // 73
      0 , 		 // 74
      0 , 		 // 75
      0 , 		 // 76
      0 , 		 // 77
      0 , 		 // 78
      0 , 		 // 79
      0 , 		 // 80
      0 , 		 // 81
      0 , 		 // 82
      0 , 		 // 83
      0 , 		 // 84
      0 , 		 // 85
      0 , 		 // 86
      0 , 		 // 87
      0 , 		 // 88
      0 , 		 // 89
      0 , 		 // 90
      0 , 		 // 91
      0 , 		 // 92
      0 , 		 // 93
      0 , 		 // 94
      0 , 		 // 95
      0 , 		 // 96
      0 , 		 // 97
      0 , 		 // 98
      0 , 		 // 99
      0 , 		 // 100
      0 , 		 // 101
      0 , 		 // 102
      0 , 		 // 103
      0 , 		 // 104

};







