# libk/Makefile

ARCH ?= i686
TARGET := $(ARCH)-elf

CC := $(TARGET)-gcc
AR := $(TARGET)-ar

BUILD_DIR := build
INCLUDE_DIR := include
KERNEL_INCLUDE_DIR := ../include
ARCH_INCLUDE_DIR := ../arch/$(ARCH)/include

LIBK := $(BUILD_DIR)/libk.a

CPPFLAGS := \
	-I$(INCLUDE_DIR) \
	-I$(KERNEL_INCLUDE_DIR) \
	-I$(ARCH_INCLUDE_DIR) \
	-D__is_libk

CFLAGS := \
	-std=gnu99 \
	-ffreestanding \
	-O2 \
	-Wall \
	-Wextra

ARFLAGS := rcs

C_SOURCES := $(shell find . -type f -name '*.c' \
	! -path './include/*' \
	! -path './build/*')

OBJECTS := $(patsubst ./%.c,$(BUILD_DIR)/%.o,$(C_SOURCES))

.PHONY: all clean

all: $(LIBK)

$(LIBK): $(OBJECTS)
	@mkdir -p $(dir $@)
	$(AR) $(ARFLAGS) $@ $^

$(BUILD_DIR)/%.o: ./%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)
