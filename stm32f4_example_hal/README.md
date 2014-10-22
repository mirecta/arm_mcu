HAL example
===========

**Several files was coppied from  templates:**

File | Decription
-----|-----------
**system_stm32f4xx.c** | This file contains SystemInit() which is called at startup just after reset and
before branching to the main program. It does not configure the system
clock at startup (contrary to the standard library). This is to be done using
the HAL APIs in the user files.
It allows to :

- relocate the vector table in internal SRAM.

- configure FSMC/FMC peripheral (when available) to use as
data memory the external SRAM or SDRAM mounted on the
evaluation board.


