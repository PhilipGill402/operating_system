ARCH := i686
TARGET := $(ARCH)-elf

CC := $(TARGET)-gcc

MAKEFILE_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
COMPOSITOR_DIR := $(patsubst %/,%,$(MAKEFILE_DIR))

SYSROOT := $(abspath $(COMPOSITOR_DIR)/../sysroot)
OUTDIR := $(abspath $(COMPOSITOR_DIR)/../fs)
BUILDDIR := $(abspath $(COMPOSITOR_DIR)/../build/compositor)

SRC_DIR := $(COMPOSITOR_DIR)/src
INCLUDE_DIR := $(COMPOSITOR_DIR)/include

OUT := $(OUTDIR)/compositor

CRT0 := $(SYSROOT)/usr/lib/crt0.o
LIBGFX := $(SYSROOT)/usr/lib/libgfx.a
LIBC := $(SYSROOT)/usr/lib/libc.a

CFLAGS := -std=gnu99 -ffreestanding -O2 -Wall -Wextra -m32
CFLAGS += -I$(INCLUDE_DIR)
CFLAGS += -I$(SYSROOT)/usr/include
CFLAGS += -MMD -MP

LDFLAGS := -nostdlib -nostartfiles -ffreestanding -m32

SRCS := $(shell find $(SRC_DIR) -name '*.c')
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILDDIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

.PHONY: all clean print dirs

all: dirs $(OUT)

dirs:
	mkdir -p $(BUILDDIR)
	mkdir -p $(OUTDIR)

$(BUILDDIR)/%.o: $(SRC_DIR)/%.c | dirs
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT): $(OBJS) $(CRT0) $(LIBGFX) $(LIBC) | dirs
	$(CC) $(LDFLAGS) $(CRT0) $(OBJS) $(LIBGFX) $(LIBC) -lgcc -o $@

print:
	@echo COMPOSITOR_DIR=$(COMPOSITOR_DIR)
	@echo SYSROOT=$(SYSROOT)
	@echo OUTDIR=$(OUTDIR)
	@echo BUILDDIR=$(BUILDDIR)
	@echo SRC_DIR=$(SRC_DIR)
	@echo INCLUDE_DIR=$(INCLUDE_DIR)
	@echo SRCS=$(SRCS)
	@echo OBJS=$(OBJS)
	@echo OUT=$(OUT)

clean:
	rm -rf $(BUILDDIR)
	rm -f $(OUT)

-include $(DEPS)
