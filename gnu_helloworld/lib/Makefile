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
SOURCEDIR = src

INCLUDEFLAGS = -I .  -I ./inc -I ./src/CMSIS 
CCFLAGS = -mlittle-endian -mthumb -mcpu=cortex-m3 -mfloat-abi=softfp -g
BUILDDIR = objs

CFLAGS = $(INCLUDEFLAGS) $(CCFLAGS)
SOURCES := $(shell find  $(SOURCEDIR) -name '*.c')
SOURCESASM := $(shell find  $(SOURCEDIR) -name '*.s')

OBJECTS := $(addprefix $(BUILDDIR)/,$(SOURCES:%.c=%.o))
OBJECTS += $(addprefix $(BUILDDIR)/,$(SOURCESASM:%.s=%.o))

all: $(BINARY)

clean:
	$(V)rm -rf ./objs
	$(V)rm $(BINARY)

$(BINARY): $(OBJECTS)
	$(V)$(AR) rcs $(BINARY) $(OBJECTS)

$(BUILDDIR)/%.o : %.s
	@mkdir -p $(@D)
	$(V)$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/%.o : %.c
	@mkdir -p $(@D)
	$(V)$(CC) $(CFLAGS) -c $< -o $@

