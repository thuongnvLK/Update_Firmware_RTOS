##########################################################################################################################
# File automatically-generated by tool: [projectgenerator] version: [3.7.1] date: [Mon Apr 13 10:53:27 ICT 2020]
##########################################################################################################################

######################################
# Target settings
######################################
TARGET = Tool

######################################
# Build configuration
######################################
DEBUG = 1
OPT = -Os  # No optimization for easier debugging

#######################################
# Paths
#######################################
BUILD_DIR = build

######################################
# Source files
######################################
C_SOURCES =  \
Src/main.c \
Src/stm32f10x_it.c \
Drivers/STM32F10x_StdPeriph_Driver/src/misc.c \
Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_adc.c \
Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_bkp.c \
Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_can.c \
Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_cec.c \
Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_crc.c \
Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_dac.c \
Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_dbgmcu.c \
Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_dma.c \
Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_exti.c \
Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_flash.c \
Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_fsmc.c \
Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_gpio.c \
Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_i2c.c \
Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_iwdg.c \
Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_pwr.c \
Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_rcc.c \
Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_rtc.c \
Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_sdio.c \
Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_spi.c \
Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_tim.c \
Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_usart.c \
Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_wwdg.c \
Src/system_stm32f10x.c  \
Src/uart.c \
Src/syscalls.c

ASM_SOURCES =  \
startup_stm32f10x_md.s

######################################
# Binary tools
######################################
PREFIX = arm-none-eabi-
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
endif
HEX = $(CP) -O ihex #--gap-fill=0xFF --pad-to 0x1000 # Đã cập nhật lệnh
# HEX = $(CP) -O ihex --gap-fill=0xFF --pad-to=0x[độ_dài_được_giới_hạn]
BIN = $(CP) -O binary -S

######################################
# Compiler flags
######################################
CPU = -mcpu=cortex-m3
MCU = $(CPU) -mthumb

C_DEFS =  \
-DSTM32F10X_MD \
-DUSE_STDPERIPH_DRIVER

C_INCLUDES =  \
-IInc \
-Iinclude \
-IDrivers/STM32F10x_StdPeriph_Driver/inc \
-IDrivers/CMSIS/CM3/DeviceSupport/ST/STM32F10x \
-IDrivers/CMSIS/CM3/CoreSupport \
-I"C:/Program Files (x86)/Arm GNU Toolchain arm-none-eabi/13.3 rel1/arm-none-eabi/include"

ASFLAGS = $(MCU) -Wall -fdata-sections -ffunction-sections

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif

CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"

######################################
# Linker flags
######################################
LDSCRIPT = STM32F103C8Tx_FLASH.ld
LIBS = -lc -lm -lrdimon

LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections -u _printf_float

######################################
# Default build action
######################################
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin $(BUILD_DIR)/$(TARGET).hala

######################################
# Build the application
######################################
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET).elf | $(BUILD_DIR)
	$(HEX) $< $@

$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf | $(BUILD_DIR)
	$(BIN) $< $@

$(BUILD_DIR)/$(TARGET).hala: $(BUILD_DIR)/$(TARGET).hex
	hala $(TARGET).hex -o $(BUILD_DIR)/$(TARGET).hala

$(BUILD_DIR):
	mkdir $@


write: $(BUILD_DIR)/$(TARGET).hala
	hala -w $(TARGET)


######################################
# Flash the binary to the STM32
######################################
flash: $(BUILD_DIR)/$(TARGET).bin
	st-flash write $< 0x08000000

######################################
# Clean up build files
######################################
clean:
	-rm -fR $(BUILD_DIR)

######################################
# Handle dependencies
######################################
-include $(wildcard $(BUILD_DIR)/*.d)

# *** EOF ***






# ##########################################################################################################################
# # File automatically-generated by tool: [projectgenerator] version: [3.7.1] date: [Mon Apr 13 10:53:27 ICT 2020]
# ##########################################################################################################################

# # ------------------------------------------------
# # Generic Makefile (based on gcc)
# #
# # ChangeLog :
# #	2017-02-10 - Several enhancements + project update mode
# #   2015-07-22 - first version
# # ------------------------------------------------

# ##################################################
# # Edited by
# ##################################################

# ######################################
# # target
# ######################################
# TARGET = Tool


# ######################################
# # building variables
# ######################################
# # debug build?
# DEBUG = 1
# # optimization
# OPT = -O0  # Không tối ưu hóa để dễ debug


# #######################################
# # paths
# #######################################
# # Build path
# BUILD_DIR = build

# ######################################
# # source
# ######################################
# # C sources
# C_SOURCES =  \
# Src/main.c \
# Src/stm32f10x_it.c \
# Drivers/STM32F10x_StdPeriph_Driver/src/misc.c \
# Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_adc.c \
# Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_bkp.c \
# Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_can.c \
# Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_cec.c \
# Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_crc.c \
# Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_dac.c \
# Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_dbgmcu.c \
# Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_dma.c \
# Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_exti.c \
# Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_flash.c \
# Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_fsmc.c \
# Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_gpio.c \
# Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_i2c.c \
# Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_iwdg.c \
# Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_pwr.c \
# Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_rcc.c \
# Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_rtc.c \
# Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_sdio.c \
# Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_spi.c \
# Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_tim.c \
# Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_usart.c \
# Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_wwdg.c \
# Src/system_stm32f10x.c  \
# Src/uart.c \
# Src/syscalls.c
# # ASM sources
# ASM_SOURCES =  \
# startup_stm32f10x_md.s

# #######################################
# # binaries
# #######################################
# PREFIX = arm-none-eabi-
# # The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# # either it can be added to the PATH environment variable.
# ifdef GCC_PATH
# CC = $(GCC_PATH)/$(PREFIX)gcc
# AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
# CP = $(GCC_PATH)/$(PREFIX)objcopy
# SZ = $(GCC_PATH)/$(PREFIX)size
# else
# CC = $(PREFIX)gcc
# AS = $(PREFIX)gcc -x assembler-with-cpp
# CP = $(PREFIX)objcopy
# SZ = $(PREFIX)size
# endif
# HEX = $(CP) -O ihex
# BIN = $(CP) -O binary -S
 
# #######################################
# # CFLAGS
# #######################################
# # cpu
# CPU = -mcpu=cortex-m3

# # fpu
# # NONE for Cortex-M0/M0+/M3

# # float-abi


# # mcu
# MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# # macros for gcc
# # AS defines
# AS_DEFS = 

# # C defines
# C_DEFS =  \
# -DSTM32F10X_MD \
# -DUSE_STDPERIPH_DRIVER


# # AS includes
# AS_INCLUDES = 

# # C includes
# C_INCLUDES =  \
# -IInc \
# -Iinclude \
# -IDrivers/STM32F10x_StdPeriph_Driver/inc \
# -IDrivers/CMSIS/CM3/DeviceSupport/ST/STM32F10x \
# -IDrivers/CMSIS/CM3/CoreSupport \
# -I"C:/Program Files (x86)/Arm GNU Toolchain arm-none-eabi/13.3 rel1/arm-none-eabi/include"

# ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

# CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

# ifeq ($(DEBUG), 1)
# CFLAGS += -g -gdwarf-2
# endif


# # Generate dependency information
# CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"


# #######################################
# # LDFLAGS
# #######################################
# # link script
# LDSCRIPT = STM32F103C8Tx_FLASH.ld

# # libraries
# #LIBS = -lc -lm -lnosys
# LIBS = -lc -lm -lrdimon

# LIBDIR = 
# #LDFLAGS = $(MCU) -specs=nosys.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections
# LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections -u _printf_float


# # default action: build all
# all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin


# #######################################
# # build the application
# #######################################
# # list of objects
# OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
# vpath %.c $(sort $(dir $(C_SOURCES)))
# # list of ASM program objects
# OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
# vpath %.s $(sort $(dir $(ASM_SOURCES)))

# $(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
# 	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

# $(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
# 	$(AS) -c $(CFLAGS) $< -o $@

# $(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
# 	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
# 	$(SZ) $@

# $(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
# 	$(HEX) $< $@
	
# $(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
# 	$(BIN) $< $@	
	
# $(BUILD_DIR):
# 	mkdir $@

# #######################################
# # st-flash added by
# #######################################		
# flash: $(BUILD_DIR)/$(TARGET).bin
# 	st-flash write $< 0x08000000
# #######################################
# # clean up
# #######################################
# clean:
# 	-rm -fR $(BUILD_DIR)
  
# #######################################
# # dependencies
# #######################################
# -include $(wildcard $(BUILD_DIR)/*.d)

# # *** EOF ***
