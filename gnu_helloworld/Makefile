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

BINARY = out.bin

SOURCEDIR = src
LDFLAGS = -L ./lib -lplaform
INCLUDEFLAGS = -I . -I ./lib/inc
CCFLAGS = -mcpu=cortex-m3 -mthumb -DSTM32
BUILDDIR = objs

CFLAGS = $(INCLUDEFLAGS) $(CCFLAGS)
SOURCES := $(shell find  $(SOURCEDIR) -name '*.c')
OBJECTS := $(addprefix $(BUILDDIR)/,$(SOURCES:%.c=%.o))

all: $(BINARY)

clean:
	$(V)rm -rf ./objs
	$(V)rm $(BINARY)
	$(V)make -C ./lib clean

$(BINARY): $(OBJECTS) lib/libplatform.a
	$(V)$(CC) $(CFLAGS)  $(OBJECTS) $(LDFLAGS) -o $(BINARY)

lib/libplatform.a:
	$(V)make -C ./lib

$(BUILDDIR)/%.o : %.c
	@mkdir -p $(@D)
	$(V)$(CC) $(CFLAGS) -c $< -o $@

