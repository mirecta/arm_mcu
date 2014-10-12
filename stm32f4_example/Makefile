# Name: Makefile
# Project: hid-mouse example
# Author: Christian Starkjohann
# Creation Date: 2008-04-07
# Tabsize: 4
# Copyright: (c) 2008 by OBJECTIVE DEVELOPMENT Software GmbH
# License: GNU GPL v2 (see License.txt), GNU GPL v3 or proprietary (CommercialLicense.txt)
# This Revision: $Id: Makefile 692 2008-11-07 15:07:40Z cs $

DEVICE  = STM32F411xE

FLASH = sudo st-flash

CFLAGS  = 
OBJECTS = startup_stm32f411xe.o system.o main.o
DEPS = $(OBJECTS:.o=.d)

COMPILE = arm-none-eabi-gcc -mcpu=cortex-m4 -Os -mlittle-endian -mthumb -mthumb-interwork -Wall -D${DEVICE} -I/home/miro/agct/STM32Cube_FW_F4_V1.3.0/Drivers/CMSIS/Device/ST/STM32F4xx/Include -I/home/miro/agct/STM32Cube_FW_F4_V1.3.0/Drivers/CMSIS/Include

all: hex bin

# symbolic targets:
help:
	@echo "This Makefile has no default rule. Use one of the following:"
	@echo "make hex ....... to build main.hex"
	@echo "make program ... to flash fuses and firmware"
	@echo "make fuse ...... to flash the fuses"
	@echo "make flash ..... to flash the firmware (use this on metaboard)"
	@echo "make clean ..... to delete objects and hex file"

hex: main.hex 

bin: main.bin

program: flash fuse

# rule for uploading firmware:
flash: main.bin
	$(FLASH) --reset write main.bin  0x8000000

fuses:
	$(AVRDUDE) -U lfuse:w:0x94:m -U hfuse:w:0xd9:m

# rule for deleting dependent files (those which can be built by Make):
clean:
	rm -f main.bin  main.hex main.lst main.obj main.cof main.list main.map main.eep.hex main.elf *.o *.d 

# Generic rule for compiling C files:
%.o: %.c
	$(COMPILE) -c $< -o $@
	$(COMPILE) -MM $*.c > $*.d 

%.o: %.cc
	$(COMPILE) -c $< -o $@
	$(COMPILE) -MM $*.cc > $*.d 
    
# Generic rule for assembling Assembler source files:
%.o: %.s
	$(COMPILE)    -c $< -o $@
# "-x assembler-with-cpp" should not be necessary since this is the default
# file type for the .S (with capital S) extension. However, upper case
# characters are not always preserved on Windows. To ensure WinAVR
# compatibility define the file type manually.

# Generic rule for compiling C to assembler, used for debugging only.
.c.s:
	$(COMPILE) -S $< -o $@

# file targets:


main.elf:  $(OBJECTS)	# usbdrv dependency only needed because we copy it
	$(COMPILE)  -Wl,--gc-sections -TSTM32F411RE_FLASH.ld -o main.elf $(OBJECTS)

main.hex: main.elf
	rm -f main.hex main.eep.hex
	arm-none-eabi-objcopy -Oihex main.elf main.hex

main.bin: main.elf
	rm -f main.bin
	arm-none-eabi-objcopy  -Obinary main.elf main.bin

# debugging targets:

disasm:	main.elf
	arm-none-eabi-objdump -S main.elf

cpp:
	$(COMPILE) -E main.c


-include	$(DEPS)
