# Tools
PREFIX=arm-none-eabi-
CC=$(PREFIX)gcc
CCLD=$(PREFIX)gcc
LD=$(PREFIX)ld
AR=$(PREFIX)ar
AS=$(PREFIX)as
OBJCOPY=$(PREFIX)objcopy

VERBOSE=1

ifeq ($(VERBOSE), 1)
	V=
else
	V=@
endif
	
//FILENAME:=  $(patsubst %.c,%.o,$(wildcard *.c))

BINARY = libplaform.a
LIB = "./Common/Cortex-M3/STM32F2xx"
SOURCEDIR = EMW3162 $(LIB)
LDFLAGS = "../lib/mxchipWNet_3162.lib" "./lib/mxchipWNet_cm3_sdio.a"
INCLUDEFLAGS = -I .  -I ./support -I ./EMW3162 -I ./include  -I $(LIB)/../ -I $(LIB) -I $(LIB)/STM32F2xx_Drv -I $(LIB)/STM32F2xx_Drv/STM32F2xx_StdPeriph_Driver/inc -I $(LIB)/STM32F2xx_Drv/STM32F2xx_StdPeriph_Driver/CMSIS 
CCFLAGS = -mcpu=cortex-m3 -mthumb -DSTM32
BUILDDIR = objs

CFLAGS = $(INCLUDEFLAGS) $(CCFLAGS)
SOURCES := $(shell find  $(SOURCEDIR) -name '*.c')
OBJECTS := $(addprefix $(BUILDDIR)/,$(SOURCES:%.c=%.o))

all: $(BINARY)

$(BINARY): $(OBJECTS)
	$(V)$(AR) rcs $(BINARY) $(OBJECTS)

$(BUILDDIR)/%.o : %.c
	$(V)$(CC) $(CFLAGS) -c $< -o $@

