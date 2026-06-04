ARCH := i686
TARGET := $(ARCH)-elf

CC := $(TARGET)-gcc

MAKEFILE_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
SYSROOT := $(abspath $(MAKEFILE_DIR)/../sysroot)
OUTDIR := $(abspath $(MAKEFILE_DIR)/../fs/)

CFLAGS := -std=gnu99 -ffreestanding -O2 -Wall -Wextra -m32 -I$(SYSROOT)/usr/include
LDFLAGS := -nostdlib -nostartfiles -ffreestanding -m32

CRT0 := $(SYSROOT)/usr/lib/crt0.o
LIBC := $(SYSROOT)/usr/lib/libc.a

C_SOURCES := $(notdir $(wildcard $(MAKEFILE_DIR)/*.c))
PROGS := $(addprefix $(OUTDIR)/,$(C_SOURCES:.c=))

.PHONY: all clean print

all: $(PROGS)

print:
	@echo MAKEFILE_DIR=$(MAKEFILE_DIR)
	@echo SYSROOT=$(SYSROOT)
	@echo OUTDIR=$(OUTDIR)
	@echo C_SOURCES=$(C_SOURCES)
	@echo PROGS=$(PROGS)

$(OUTDIR)/%: $(MAKEFILE_DIR)/%.c $(CRT0) $(LIBC)
	mkdir -p $(OUTDIR)
	$(CC) $(CFLAGS) $(LDFLAGS) $< $(CRT0) $(LIBC) -o $@

clean:
	rm -f $(PROGS)
