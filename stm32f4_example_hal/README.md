HAL example
===========

**Several files was coppied from  templates:**

*system_stm32f4xx.c*

This file contains SystemInit() which is called at startup just after reset and </br>
before branching to the main program. It does not configure the system
clock at startup (contrary to the standard library). This is to be done using
the HAL APIs in the user files.
It allows to :

- relocate the vector table in internal SRAM.

- configure FSMC/FMC peripheral (when available) to use as
data memory the external SRAM or SDRAM mounted on the
evaluation board.

*startup_stm32f4xx.s*

Toolchain specific file that contains reset handler and exception vectors.
For some toolchains, it allows adapting the stack/heap size to fit the
application requirements.

*stm32f4xx_hal_msp.c*

This file contains the MSP initialization and de-initialization (main routine
and callbacks) of the peripheral used in the user application.

*stm32f4xx_hal_conf.h*

This file allows the user to customize the HAL drivers for a specific
application.
It is not mandatory to modify this configuration. The application can use the
default configuration without any modification.

*stm32f4xx_it.c/.h*

This file contains the exceptions handler and peripherals interrupt service
routine, and calls HAL_IncTick() at regular time intervals to increment a
local variable (declared in stm32f4xx_hal.c) used as HAL timebase. By
default, this function is called each 1ms in Systick ISR. .
The PPP_IRQHandler() routine must call HAL_PPP_IRQHandler() if an
interrupt based process is used within the application.

*main.c/.h*

This file contains the main program routine, mainly:

- the call to HAL_Init()

- assert_failed() implementation

- system clock configuration

- peripheral HAL initialization and user application code.




