##
# @file   Makefile
# @author cy023@MVMC-Lab (cyyang023@gmail.com)
# @date   2022.09.17
# @brief  Makefile for NuM487 project.

################################################################################
# User Settings
################################################################################

## Target
TARGET = main

# Upload Info.
COMPORT    ?=
UPLOAD_HEX ?= main

## MCU Info.
CPU       = -mcpu=cortex-m4
MCU       = 
FPU       = -mfpu=fpv4-sp-d16
FLOAT-ABI = -mfloat-abi=hard

## Warning Options
WARNINGS = -Wall -Werror -Wtype-limits -Wno-unused-function

## Debugging Options
DEBUG = -gdwarf-2

## Optimize Options
OPTIMIZE = -O0

## Libraries Path
LIBS    = 
LIBDIRS = 

## Include Path
C_INCLUDES  = -I.
C_INCLUDES += -ICore
C_INCLUDES += -ICore/boot
C_INCLUDES += -IDevice_Startup
C_INCLUDES += -IDrivers/CMSIS
C_INCLUDES += -IDrivers/boot
C_INCLUDES += -IDrivers/w25q128jv
C_INCLUDES += -IDrivers/Library/Device/Nuvoton_M480/Include
C_INCLUDES += -IDrivers/Library/StdDriver/inc
C_INCLUDES += -IMiddleware/LittleFS

## Source Path
C_SOURCES += $(wildcard Core/boot/*.c)
C_SOURCES += $(wildcard Device_Startup/*.c)
C_SOURCES += $(wildcard Drivers/Library/Device/Nuvoton_M480/Source/*.c)
# C_SOURCES += $(wildcard Drivers/Library/StdDriver/src/*.c)
C_SOURCES += Drivers/Library/StdDriver/src/sys.c
C_SOURCES += Drivers/Library/StdDriver/src/uart.c
C_SOURCES += Drivers/Library/StdDriver/src/gpio.c
C_SOURCES += Drivers/Library/StdDriver/src/retarget.c
C_SOURCES += Drivers/Library/StdDriver/src/clk.c
C_SOURCES += Drivers/Library/StdDriver/src/fmc.c
C_SOURCES += Drivers/Library/StdDriver/src/spi.c
C_SOURCES += $(wildcard Drivers/boot/*.c)
C_SOURCES += $(wildcard Drivers/w25q128jv/*.c)
C_SOURCES += $(wildcard Middleware/LittleFS/*.c)

ASM_SOURCES += $(wildcard Device_Startup/*.S)

################################################################################
# Project Architecture
################################################################################

## Root Path
IPATH = .

## Build Output Path
BUILD_DIR = build

## Build Reference Path
VPATH  = $(sort $(dir $(C_SOURCES)))
VPATH += $(sort $(dir $(C_APPSRCS)))
VPATH += $(sort $(dir $(C_TESTSRC)))

## User App Path
C_APPSRCS = $(wildcard Core/*.c)

## Unit Test Path
C_TESTSRC = $(wildcard UnitTest/*.c)

## Object Files
OBJECTS  = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o))) 
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.S=.o)))
APPOBJS  = $(addprefix $(BUILD_DIR)/,$(notdir $(C_APPSRCS:.c=.o)))
TESTOBJ  = $(addprefix $(BUILD_DIR)/,$(notdir $(C_TESTSRC:.c=.o)))

## Target and UnitTest HEX Files
TARGET_FILE = $(BUILD_DIR)/$(TARGET).hex
TEST_TARGET = $(TESTOBJ:.o=.hex)

### Linker script
LDSCRIPT = Device_Startup/gcc_arm.ld

################################################################################
# Toolchain
################################################################################
COMPILER_PATH ?= 
CROSS   := $(COMPILER_PATH)arm-none-eabi
CC      := $(CROSS)-gcc
AR      := $(CROSS)-ar
SIZE    := $(CROSS)-size
OBJDUMP := $(CROSS)-objdump
OBJCOPY := $(CROSS)-objcopy
NM      := $(CROSS)-nm

## Common Options for assemble, compile and link.
MCUFLAGS = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

## Compile Options
CFLAGS  = $(MCUFLAGS)
CFLAGS += -std=gnu99 $(WARNINGS) $(DEBUG) $(OPTIMIZE)
CFLAGS += -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections
CFLAGS += -Wa,-a,-ad,-alms=$(@:%.o=%.lst)
CFLAGS += -Wp,-MM,-MP,-MT,$(BUILD_DIR)/$(*F).o,-MF,$(BUILD_DIR)/$(*F).d
CFLAGS += $(C_INCLUDES)

## Assembler Options
ASMFLAGS  = $(MCUFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,$(DEBUG)

## Link Options
LDFLAGS  = $(MCUFLAGS)
LDFLAGS += -Wl,-Map=$(BUILD_DIR)/$*.map,--cref -Wl,--gc-sections
LDFLAGS += -T$(LDSCRIPT)
LDFLAGS += -specs=nosys.specs -specs=nano.specs -flto
LDFLAGS += -u _printf_float
LDFLAGS += $(LIBDIRS) $(LIBS)

## Archiver Options
# ARFLAGS = rcs

## Intel Hex file production Options
HEX_FLASH_FLAGS   = -R .eeprom -R .fuse -R .lock -R .signature
HEX_EEPROM_FLAGS  = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom=alloc,load
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings

################################################################################
# User Command
################################################################################

all: $(OBJECTS) $(APPOBJS) $(TARGET_FILE)
	@echo "========== SIZE =========="
	$(SIZE) $^

test: $(OBJECTS) $(TESTOBJ) $(TEST_TARGET)
	@echo "========== SIZE =========="
	$(SIZE) $^

macro: $(OBJECTS:.o=.i) $(APPOBJS:.o=.i) $(TESTOBJ:.o=.i)

dump: $(BUILD_DIR)/$(TARGET).lss $(TESTOBJ:.o=.lss) $(BUILD_DIR)/$(TARGET).sym $(TESTOBJ:.o=.sym)

size: $(TARGET_FILE)
	$(SIZE) $(TARGET_FILE)

clean:
	-rm -rf $(BUILD_DIR)

# download:
# 	NuLink -e all
# 	NuLink 0x180006EE -w APROM build/$(UPLOAD_HEX).hex

terminal:
	putty -serial $(COMPORT) -sercfg 38400,1,N,N

systeminfo:
	@uname -a
	@$(CC) --version

.PHONY: all test macro dump size systeminfo clean upload terminal

################################################################################
# Build The Project
################################################################################

## Preprocess
$(BUILD_DIR)/%.i: %.c Makefile | $(BUILD_DIR)
	$(CC) -E $(C_INCLUDES) $< -o $@

## Compile
$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.o: %.S Makefile | $(BUILD_DIR)
	$(CC) -c $(ASMFLAGS) $< -o $@

## Link
$(TARGET_FILE:.hex=.elf): $(OBJECTS) $(APPOBJS) Makefile
	$(CC) $(LDFLAGS) $(OBJECTS) $(APPOBJS) -o $@

$(TEST_TARGET:.hex=.elf): $(OBJECTS) $(TESTOBJ) Makefile
	$(CC) $(LDFLAGS) $(OBJECTS) $(@:%.elf=%.o) -o $@

## Intel HEX format
$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(OBJCOPY) -O ihex $(HEX_FLASH_FLAGS) $< $@

## Binary format
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(OBJCOPY) -O binary $< $@

## Archive
# $(BUILD_DIR)/libc4mrtos.a: $(OBJECTS)
# 	$(AR) rcs $@ $(OBJECTS)

## Eeprom format
$(BUILD_DIR)/%.eep: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(OBJCOPY) $(HEX_EEPROM_FLAGS) -O binary $< $@ || exit 0

## Disassemble
$(BUILD_DIR)/%.lss: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(OBJDUMP) -h -S $< > $@

## Symbol Table
$(BUILD_DIR)/%.sym: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(NM) -n $< > $@

## Make Directory
$(BUILD_DIR):
	mkdir $@

################################################################################
# dependencies
################################################################################

-include $(wildcard $(BUILD_DIR)/*.d)
