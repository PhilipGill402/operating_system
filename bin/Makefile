ARCH := i686
TARGET := $(ARCH)-elf

CC := $(TARGET)-gcc

MAKEFILE_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
SYSROOT := $(abspath $(MAKEFILE_DIR)/../sysroot)
OUTDIR := $(abspath $(MAKEFILE_DIR)/../fs)
BUILDDIR := $(abspath $(MAKEFILE_DIR)/../build/user)

CFLAGS := -std=gnu99 -ffreestanding -O2 -Wall -Wextra -m32 -I$(SYSROOT)/usr/include
CFLAGS += -MMD -MP

LDFLAGS := -nostdlib -nostartfiles -ffreestanding -m32

CRT0 := $(SYSROOT)/usr/lib/crt0.o
LIBC := $(SYSROOT)/usr/lib/libc.a
LIBGFX := $(SYSROOT)/usr/lib/libgfx.a

C_SOURCES := $(notdir $(wildcard $(MAKEFILE_DIR)/*.c))

OBJS := $(addprefix $(BUILDDIR)/,$(C_SOURCES:.c=.o))
DEPS := $(OBJS:.o=.d)

PROGS := $(addprefix $(OUTDIR)/,$(C_SOURCES:.c=))

.PHONY: all clean print

all: $(PROGS)

print:
	@echo MAKEFILE_DIR=$(MAKEFILE_DIR)
	@echo SYSROOT=$(SYSROOT)
	@echo OUTDIR=$(OUTDIR)
	@echo BUILDDIR=$(BUILDDIR)
	@echo C_SOURCES=$(C_SOURCES)
	@echo OBJS=$(OBJS)
	@echo PROGS=$(PROGS)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(OUTDIR):
	mkdir -p $(OUTDIR)

$(BUILDDIR)/%.o: $(MAKEFILE_DIR)/%.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OUTDIR)/%: $(BUILDDIR)/%.o $(CRT0) $(LIBGFX) $(LIBC) | $(OUTDIR)
	$(CC) $(LDFLAGS) $(CRT0) $< $(LIBGFX) $(LIBC) -o $@

clean:
	rm -rf $(BUILDDIR)
	rm -f $(PROGS)

-include $(DEPS)
