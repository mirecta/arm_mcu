
BUILD_DIR:=build
DEVICE := STM32F10X_HD
CPU_FREQ := 72000000

############# uGFX ######################
GFXLIB := src/ugfx
export GFXLIB
include src/ugfx/gfx.mk

GFXOBJ := $(patsubst %.c,%.o,$(GFXSRC))


#########################################


#CUBE_DRV_PATH := /home/miro/agct/STM32Cube_FW_F4_V1.3.0/Drivers
#HAL_PATH :=  ${CUBE_DRV_PATH}/STM32F4xx_HAL_Driver
HAL_PATH := src/hal
#MODULES := src $(HAL_PATH)/Src

MODULES:= src  src/core src/util  $(HAL_PATH)/src
INCLUDES:= include $(HAL_PATH)/inc $(GFXINC)
EXCLUDES:= 

DEFINES:= USE_STDPERIPH_DRIVER $(DEVICE) CPU_FREQ=$(CPU_FREQ) GFX_USE_OS_RAW32=TRUE
INCLUDES += . $(MODULES)

SRC:= $(wildcard *.c *.cc *.s)
SRC += $(foreach sdir,$(MODULES),$(wildcard $(sdir)/*.c $(sdir)/*.cc $(sdir)/*.s))
SRC:= $(filter-out $(EXCLUDES),$(SRC))

OBJ:= $(patsubst %.c,%.o,$(SRC))
OBJ:= $(patsubst %.cc,%.o,$(OBJ))
OBJ:= $(patsubst %.s,%.o,$(OBJ))
OBJ += $(GFXOBJ)
OBJ:= $(patsubst %.o, build/%.o, $(notdir $(OBJ)))


VPATH:=$(MODULES) $(GFXVPATH)

SOFT_FLOAT := -mfloat-abi=soft
HARD_FLOAT := -mfpu=fpv4-sp-d16 -mfloat-abi=hard

CFLAGS :=    $(SOFT_FLOAT) -g -ggdb -mcpu=cortex-m3 -O0 -mlittle-endian -mthumb -mthumb-interwork  -Wall 
CFLAGS += $(addprefix -I ,$(INCLUDES))
CFLAGS += $(addprefix -D ,$(DEFINES))

LDFLAGS :=   $(SOFT_FLOAT) -g -ggdb --specs=nosys.specs -mcpu=cortex-m3 -mlittle-endian -mthumb -mthumb-interwork   -D${DEVICE} -Tstm32_flash.ld -Wl,--gc-sections 

DEPS = $(OBJ:.o=.d)

CC := arm-none-eabi-gcc 
CXX := arm-none-eabi-g++
LD := arm-none-eabi-ld
OBJCOPY := arm-none-eabi-objcopy
OBJDUMP := arm-none-eabi-objdump

FLASH := sudo st-flash



all: checkdirs hex bin
	@echo $(OBJ)
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
	$(FLASH) --reset write $(BUILD_DIR)/main.bin  0x8000000 2>&1


# Generic rule for compiling C files:
$(BUILD_DIR)/%.o: %.c
	$(CC) -std=c99   $(CFLAGS)  -c $< -o $@
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
	$(CXX)  $(LDFLAGS)  -o $(BUILD_DIR)/main.elf $(OBJ)

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
