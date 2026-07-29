# libc/Makefile

ARCH ?= i686
TARGET := $(ARCH)-elf

CC := $(TARGET)-gcc
AR := $(TARGET)-ar

BUILD_DIR := build
INCLUDE_DIR := include
KERNEL_INCLUDE_DIR := ../include

LIBC := $(BUILD_DIR)/libc.a
CRT0 := $(BUILD_DIR)/crt0.o
CRT0_SOURCE := crt/crt0.s

CPPFLAGS := \
	-I$(INCLUDE_DIR) \
	-I$(KERNEL_INCLUDE_DIR) \
	-D__is_libc

CFLAGS := \
	-std=gnu99 \
	-ffreestanding \
	-O2 \
	-Wall \
	-Wextra \
	-fomit-frame-pointer

ASFLAGS := --32
ARFLAGS := rcs

C_SOURCES := $(shell find . -type f -name '*.c' \
	! -path './include/*' \
	! -path './build/*')

OBJECTS := $(patsubst ./%.c,$(BUILD_DIR)/%.o,$(C_SOURCES))

.PHONY: all clean

all: $(LIBC) $(CRT0)

$(LIBC): $(OBJECTS)
	@mkdir -p $(dir $@)
	$(AR) $(ARFLAGS) $@ $^

$(CRT0): $(CRT0_SOURCE)
	@mkdir -p $(dir $@)
	$(CC) -m32 -ffreestanding -c $< -o $@

$(BUILD_DIR)/%.o: ./%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)
