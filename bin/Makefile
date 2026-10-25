# =============================================================================
# Architecture and toolchain
# =============================================================================

ARCH ?= riscv64
TARGET := $(ARCH)-elf

CC := $(TARGET)-gcc

# =============================================================================
# Directory layout
# =============================================================================

MAKEFILE_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
SRC_DIR := $(patsubst %/,%,$(MAKEFILE_DIR))

PROJECT_ROOT := $(abspath $(SRC_DIR)/..)

SYSROOT := $(PROJECT_ROOT)/sysroot
OUTDIR := $(PROJECT_ROOT)/fs/bin
BUILDDIR := $(PROJECT_ROOT)/build/$(ARCH)/user

# =============================================================================
# Runtime and libraries
# =============================================================================

CRT0 := $(SYSROOT)/usr/lib/crt0.o
LIBC := $(SYSROOT)/usr/lib/libc.a
LIBGFX := $(SYSROOT)/usr/lib/libgfx.a

# =============================================================================
# Compiler and linker flags
# =============================================================================

CPPFLAGS := \
	-I$(SYSROOT)/usr/include

CFLAGS := \
	-std=gnu99 \
	-ffreestanding \
	-O2 \
	-g \
	-Wall \
	-Wextra \
	-MMD \
	-MP \
	-march=rv64imac \
	-mabi=lp64

LDFLAGS := \
	-nostdlib \
	-march=rv64imac \
	-mabi=lp64

# =============================================================================
# Source discovery
# =============================================================================

C_SOURCES := $(wildcard $(SRC_DIR)/*.c)

PROG_NAMES := $(basename $(notdir $(C_SOURCES)))

OBJS := $(addprefix $(BUILDDIR)/,$(addsuffix .o,$(PROG_NAMES)))
DEPS := $(OBJS:.o=.d)

PROGS := $(addprefix $(OUTDIR)/,$(PROG_NAMES))

# =============================================================================
# Targets
# =============================================================================

.PHONY: all clean print dirs check-sysroot

all: check-sysroot $(PROGS)

dirs:
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(OUTDIR)

# =============================================================================
# Verify sysroot
# =============================================================================

check-sysroot:
	@test -f $(CRT0) || \
		(echo "Missing $(CRT0). Build and install libc first."; exit 1)

	@test -f $(LIBC) || \
		(echo "Missing $(LIBC). Build and install libc first."; exit 1)

	@test -f $(LIBGFX) || \
		(echo "Missing $(LIBGFX). Build and install libgfx first."; exit 1)

# =============================================================================
# Compile user-program source files
# =============================================================================

$(BUILDDIR)/%.o: $(SRC_DIR)/%.c | dirs
	$(CC) \
		$(CPPFLAGS) \
		$(CFLAGS) \
		-c $< \
		-o $@

# =============================================================================
# Link user programs
# =============================================================================

$(OUTDIR)/%: $(BUILDDIR)/%.o $(CRT0) $(LIBGFX) $(LIBC) | dirs
	$(CC) \
		$(LDFLAGS) \
		$(CRT0) \
		$< \
		$(LIBGFX) \
		$(LIBC) \
		-lgcc \
		-o $@

# =============================================================================
# Debug output
# =============================================================================

print:
	@echo "ARCH=$(ARCH)"
	@echo "TARGET=$(TARGET)"
	@echo "CC=$(CC)"
	@echo
	@echo "MAKEFILE_DIR=$(MAKEFILE_DIR)"
	@echo "PROJECT_ROOT=$(PROJECT_ROOT)"
	@echo "SRC_DIR=$(SRC_DIR)"
	@echo "SYSROOT=$(SYSROOT)"
	@echo "OUTDIR=$(OUTDIR)"
	@echo "BUILDDIR=$(BUILDDIR)"
	@echo
	@echo "CRT0=$(CRT0)"
	@echo "LIBC=$(LIBC)"
	@echo "LIBGFX=$(LIBGFX)"
	@echo
	@echo "C_SOURCES=$(C_SOURCES)"
	@echo "PROG_NAMES=$(PROG_NAMES)"
	@echo "OBJS=$(OBJS)"
	@echo "DEPS=$(DEPS)"
	@echo "PROGS=$(PROGS)"

# =============================================================================
# Cleanup
# =============================================================================

clean:
	rm -rf $(BUILDDIR)
	rm -f $(PROGS)

# =============================================================================
# Automatically generated dependencies
# =============================================================================

-include $(DEPS)
