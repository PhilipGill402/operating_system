ARCH := i686
TARGET := $(ARCH)-elf

CC := $(TARGET)-gcc
AR := $(TARGET)-ar

LIBC_DIR := .
BUILD_DIR := ../build/libc
LIBK := ../build/libk.a
INC_DIR := include

CFLAGS := -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I$(INC_DIR) -D__is_libk

C_SOURCES := $(shell find . -type f -name '*.c' ! -path './include/*')
OBJECTS := $(patsubst ./%.c,$(BUILD_DIR)/%.o,$(C_SOURCES))

.PHONY: all clean dirs

all: $(LIBK)

dirs:
	mkdir -p $(BUILD_DIR)

$(LIBK): $(OBJECTS)
	mkdir -p ../build
	$(AR) rcs $@ $^

$(BUILD_DIR)/%.o: ./%.c | dirs
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(LIBK)
