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

BINARY = out
LDSCRIPT=./ldscript/stm32.ld
SOURCEDIR = src lib/src
LDFLAGS = -T$(LDSCRIPT) -nostartfiles  #-lplaform 
INCLUDEFLAGS = -I . -I ./lib/inc
CCFLAGS = -fno-common -O0 -g -mcpu=cortex-m3 -mthumb -Wall
BUILDDIR = objs

CFLAGS = $(INCLUDEFLAGS) $(CCFLAGS)
SOURCES := $(shell find  $(SOURCEDIR) -maxdepth 1 -name '*.c')
SOURCESASM := $(shell find  $(SOURCEDIR) -maxdepth 1 -name '*.s')

OBJECTS := $(addprefix $(BUILDDIR)/,$(SOURCES:%.c=%.o))
OBJECTS += $(addprefix $(BUILDDIR)/,$(SOURCESASM:%.s=%.o))

all: $(BINARY)

clean:
	$(V)rm -rf ./objs
	$(V)rm $(BINARY).bin.elf $(BINARY).bin.gcc $(BINARY).bin
	

$(BINARY): $(OBJECTS) 
	$(V)$(CC) $(CFLAGS)  $(OBJECTS) $(LDFLAGS) -o $@.bin.gcc
	$(V)$(LD) $(LDLIBS) $(LDFLAGS) -o $@.bin.elf $(OBJECTS) 
	$(V)$(OBJCOPY) -Obinary $@.bin.elf $@.bin


$(BUILDDIR)/%.o : %.s
	@mkdir -p $(@D)
	$(V)$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/%.o : %.c
	$(V)mkdir -p $(@D)
	$(V)$(CC) $(CFLAGS) -c $< -o $@

