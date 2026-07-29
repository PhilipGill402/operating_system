# libgfx/Makefile

ARCH ?= i686
TARGET := $(ARCH)-elf

CC := $(TARGET)-gcc
AR := $(TARGET)-ar

BUILD_DIR := build
SOURCE_DIR := src
INCLUDE_DIR := include
SYSROOT ?= ../sysroot

LIBGFX := $(BUILD_DIR)/libgfx.a

CPPFLAGS := \
	-I$(INCLUDE_DIR) \
	-I$(SYSROOT)/usr/include

CFLAGS := \
	-std=gnu99 \
	-ffreestanding \
	-O2 \
	-Wall \
	-Wextra

ARFLAGS := rcs

C_SOURCES := $(shell find $(SOURCE_DIR) -type f -name '*.c')
OBJECTS := $(patsubst $(SOURCE_DIR)/%.c,$(BUILD_DIR)/%.o,$(C_SOURCES))
HEADERS := $(shell find $(INCLUDE_DIR) -type f -name '*.h')

.PHONY: all install clean

all: $(LIBGFX)

$(LIBGFX): $(OBJECTS)
	@mkdir -p $(dir $@)
	$(AR) $(ARFLAGS) $@ $^

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Optional installation target for user-program linking.
install: $(LIBGFX) $(HEADERS)
	mkdir -p $(SYSROOT)/usr/lib
	mkdir -p $(SYSROOT)/usr/include/gfx
	cp $(LIBGFX) $(SYSROOT)/usr/lib/
	cp $(HEADERS) $(SYSROOT)/usr/include/gfx/

clean:
	rm -rf $(BUILD_DIR)
