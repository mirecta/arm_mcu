
BUILD_DIR:=build
DEVICE := STM32F411xE

CUBE_DRV_PATH := /home/miro/agct/STM32Cube_FW_F4_V1.3.0/Drivers/
HAL_PATH :=  ${CUBE_DRV_PATH}/STM32F4xx_HAL_Driver

#MODULES := $(HAL_PATH)/Src/
NODULES:=
INCLUDES:= ${HAL_PATH}/Inc ${CUBE_DRV_PATH}/CMSIS/Device/ST/STM32F4xx/Include ${CUBE_DRV_PATH}/CMSIS/Include


INCLUDES += . $(MODULES)

SRC:= $(wildcard *.c *.cc *.s)
SRC += $(foreach sdir,$(MODULES),$(wildcard $(sdir)/*.c $(sdir)/*.cc $(sdir)/*.s))

OBJ:= $(patsubst %.c,%.o,$(SRC))
OBJ:= $(patsubst %.cc,%.o,$(OBJ))
OBJ:= $(patsubst %.s,%.o,$(OBJ))
OBJ:= $(patsubst %.o, build/%.o, $(notdir $(OBJ)))
VPATH:=$(MODULES)

CFLAGS := -mcpu=cortex-m4 -Os -mlittle-endian -mthumb -mthumb-interwork -Wall -D${DEVICE}
CFLAGS += $(addprefix -I ,$(INCLUDES))
LDFLAGS := -Wl,--gc-sections -TSTM32F411RE_FLASH.ld

DEPS = $(OBJ:.o=.d)

CC := arm-none-eabi-gcc 
OBJCOPY := arm-none-eabi-objcopy
OBJDUMP := arm-none-eabi-objdump

FLASH := sudo st-flash



all: checkdirs hex bin
	@echo $(DEPS)

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
