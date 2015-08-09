
BUILD_DIR:=build
DEVICE := STM32F103xB
CPU_FREQ:=72000000
HSE_VALUE:=8000000

RTOS_ARCH=ARM_CM3

CUBE_DRV_PATH := Drivers
RTOS_PATH := rtos

HAL_PATH :=  ${CUBE_DRV_PATH}/STM32F1xx_HAL_Driver

MODULES := src $(HAL_PATH)/Src $(RTOS_PATH) $(RTOS_PATH)/portable/GCC/$(RTOS_ARCH) $(RTOS_PATH)/CMSIS_RTOS  $(RTOS_PATH)/portable/MemMang
#MODULES:= src
INCLUDES:= include  ${HAL_PATH}/Inc ${CUBE_DRV_PATH}/CMSIS/Device/ST/STM32F1xx/Include ${CUBE_DRV_PATH}/CMSIS/Include $(RTOS_PATH)/include  $(RTOS_PATH)/CMSIS_RTOS $(RTOS_PATH)/portable/GCC/$(RTOS_ARCH)
#EXCLUDES:= $(HAL_PATH)/Src/stm32f4xx_hal_msp_template.c

LDSCRIPT:= STM32F103XB_FLASH.ld
ARCH:=cortex-m3


DEFINES:= DEBUG $(DEVICE) CPU_FREQ=$(CPU_FREQ) HSE_VALUE=$(HSE_VALUE)


#=================================================================


INCLUDES += . $(MODULES)

SRC:= $(wildcard *.c *.cc *.s)
SRC += $(foreach sdir,$(MODULES),$(wildcard $(sdir)/*.c $(sdir)/*.cc $(sdir)/*.s))
SRC:= $(filter-out $(EXCLUDES),$(SRC))

OBJ:= $(patsubst %.c,%.o,$(SRC))
OBJ:= $(patsubst %.cc,%.o,$(OBJ))
OBJ:= $(patsubst %.s,%.o,$(OBJ))
OBJ:= $(patsubst %.o, build/%.o, $(notdir $(OBJ)))
VPATH:=$(MODULES)

CFLAGS := -ggdb -msoft-float -mcpu=${ARCH} -Os -mlittle-endian -mthumb -mthumb-interwork -Wall 
CFLAGS += $(addprefix -I ,$(INCLUDES))
CFLAGS += $(addprefix -D ,$(DEFINES))

LDFLAGS := -mcpu=${ARCH} -mlittle-endian -mthumb -mthumb-interwork  -D${DEVICE} -T${LDSCRIPT} -Wl,--gc-sections

DEPS = $(OBJ:.o=.d)

CC := arm-none-eabi-gcc 
CXX := arm-none-eabi-g++
LD := arm-none-eabi-ld
OBJCOPY := arm-none-eabi-objcopy
OBJDUMP := arm-none-eabi-objdump

FLASH := sudo st-flash



all: checkdirs hex bin

# symbolic targets:
help:
	@echo "This Makefile has no default rule. Use one of the following:"
	@echo "make hex ....... to build main.hex"
	@echo "make program ... to flash fuses and firmware"
	@echo "make flash ..... to flash the firmware (use this on metaboard)"
	@echo "make clean ..... to delete objects and hex file"

hex: $(BUILD_DIR)/main.hex 

bin: $(BUILD_DIR)/main.bin


# rule for uploading firmware:
flash: $(BUILD_DIR)/main.bin
	$(FLASH) --reset write $(BUILD_DIR)/main.bin  0x8000000 


# Generic rule for compiling C files:
$(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
	$(CC) $(CFLAGS) -MM $< > $(BUILD_DIR)/$*.d 

$(BUILD_DIR)/%.o: %.cc
	$(CC) $(CFLAGS) -c $< -o $@
	$(CC) $(CFLAGS) -MM $< > $(BUILD_DIR)/$*.d 
    
# Generic rule for assembling Assembler source files:
$(BUILD_DIR)/%.o: %.s
	$(CC) $(CFLAGS)  -c $< -o $@
	$(CC) $(CFLAGS) -MM $< > $(BUILD_DIR)/$*.d 


# file targets:


$(BUILD_DIR)/main.elf:  $(OBJ)	
	$(CC)  $(LDFLAGS)  -o $(BUILD_DIR)/main.elf $(OBJ)

$(BUILD_DIR)/main.hex: $(BUILD_DIR)/main.elf
	rm -f $(BUILD_DIR)/main.hex $(BUILD_DIR)/main.eep.hex
	$(OBJCOPY) -Oihex $(BUILD_DIR)/main.elf $(BUILD_DIR)/main.hex

$(BUILD_DIR)/main.bin: $(BUILD_DIR)/main.elf
	rm -f $(BUILD_DIR)/main.bin
	$(OBJCOPY)  -Obinary $(BUILD_DIR)/main.elf $(BUILD_DIR)/main.bin

# debugging targets:

disasm:	$(BUILD_DIR)/main.elf
	$(OBJDUMP) -S $(BUILD_DIR)/main.elf


checkdirs:$(BUILD_DIR)


$(BUILD_DIR):
	@mkdir -p $@

clean:
	@rm -rf $(BUILD_DIR)


-include	$(DEPS)
