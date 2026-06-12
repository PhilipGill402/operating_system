ARCH := i686
TARGET := $(ARCH)-elf

CC := $(TARGET)-gcc

MAKEFILE_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
SRC_DIR := $(patsubst %/,%,$(MAKEFILE_DIR))

SYSROOT := $(abspath $(SRC_DIR)/../sysroot)
OUTDIR := $(abspath $(SRC_DIR)/../fs)
BUILDDIR := $(abspath $(SRC_DIR)/../build/user)

CFLAGS := -std=gnu99 -ffreestanding -O2 -Wall -Wextra -m32
CFLAGS += -I$(SYSROOT)/usr/include
CFLAGS += -MMD -MP

LDFLAGS := -nostdlib -nostartfiles -ffreestanding -m32

CRT0 := $(SYSROOT)/usr/lib/crt0.o
LIBC := $(SYSROOT)/usr/lib/libc.a
LIBGFX := $(SYSROOT)/usr/lib/libgfx.a

C_SOURCES := $(wildcard $(SRC_DIR)/*.c)
PROG_NAMES := $(basename $(notdir $(C_SOURCES)))

OBJS := $(addprefix $(BUILDDIR)/,$(addsuffix .o,$(PROG_NAMES)))
DEPS := $(OBJS:.o=.d)
PROGS := $(addprefix $(OUTDIR)/,$(PROG_NAMES))

.PHONY: all clean print dirs

all: dirs $(PROGS)

dirs:
	mkdir -p $(BUILDDIR)
	mkdir -p $(OUTDIR)

print:
	@echo MAKEFILE_DIR=$(MAKEFILE_DIR)
	@echo SRC_DIR=$(SRC_DIR)
	@echo SYSROOT=$(SYSROOT)
	@echo OUTDIR=$(OUTDIR)
	@echo BUILDDIR=$(BUILDDIR)
	@echo C_SOURCES=$(C_SOURCES)
	@echo PROG_NAMES=$(PROG_NAMES)
	@echo OBJS=$(OBJS)
	@echo DEPS=$(DEPS)
	@echo PROGS=$(PROGS)

$(OBJS): $(BUILDDIR)/%.o: $(SRC_DIR)/%.c | dirs
	$(CC) $(CFLAGS) -c $< -o $@

$(PROGS): $(OUTDIR)/%: $(BUILDDIR)/%.o $(CRT0) $(LIBGFX) $(LIBC) | dirs
	$(CC) $(LDFLAGS) $(CRT0) $< $(LIBGFX) $(LIBC) -lgcc -o $@

clean:
	rm -rf $(BUILDDIR)
	rm -f $(PROGS)

-include $(DEPS)
