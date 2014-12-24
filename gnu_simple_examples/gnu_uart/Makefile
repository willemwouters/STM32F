
# Put your stlink folder here so make burn will work.
STLINK=~/stlink.git

# Put your source files here (or *.c, etc)
SRCS=main.c system_stm32f2xx.c stm32f2xx_gpio.c stm32f2xx_rcc.c stm32f2xx_usart.c
SRCS += startup_stm32f2xx.s newlib_stubs.c

# Binaries will be generated with this name (.elf, .bin, .hex, etc)
PROJ_NAME=uart

# Put your STM32F2 library code directory here
STM_COMMON=./bsp/STM32F2xx_StdPeriph_Lib_V1.1.0/

# Normally you shouldn't need to change anything below this line!
#######################################################################################

CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy

CFLAGS  = -g -O0 -Wall -Tstm32_flash.ld 
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m3
CFLAGS += -msoft-float -DSTM32F2
CFLAGS += -I.

# Include files from STM libraries
CFLAGS += -I$(STM_COMMON)/Utilities/STM32F2-Discovery
CFLAGS += -I$(STM_COMMON)/Libraries/CMSIS/Include -I$(STM_COMMON)/Libraries/CMSIS/Device/ST/STM32F2xx/Include
CFLAGS += -I$(STM_COMMON)/Libraries/STM32F2xx_StdPeriph_Driver/inc

OBJS = $(SRCS:.c=.o)


.PHONY: proj

all: proj

proj: $(PROJ_NAME).elf

$(PROJ_NAME).elf: $(SRCS)
	$(CC) $(CFLAGS) $^ -o $@ 
	$(OBJCOPY) -O ihex $(PROJ_NAME).elf $(PROJ_NAME).hex
	$(OBJCOPY) -O binary $(PROJ_NAME).elf $(PROJ_NAME).bin

clean:
	rm -f *.o $(PROJ_NAME).elf $(PROJ_NAME).hex $(PROJ_NAME).bin

# Flash the STM32F4
burn: proj
	$(STLINK)/st-flash write $(PROJ_NAME).bin 0x8000000
