ARCH := i686
TARGET := $(ARCH)-elf

CC := $(TARGET)-gcc
AR := $(TARGET)-ar

BUILD_DIR := build
LIBC := $(BUILD_DIR)/libc.a
CRT := $(BUILD_DIR)/crt0.o
INC_DIR := include

CFLAGS := -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I$(INC_DIR) -D__is_libc -I../include -fomit-frame-pointer

C_SOURCES := $(shell find . -type f -name '*.c' ! -path './include/*' ! -path './build/*')
CRT_SOURCE := crt/crt0.s
OBJECTS := $(patsubst ./%.c,$(BUILD_DIR)/%.o,$(C_SOURCES))

.PHONY: all clean dirs

all: $(LIBC) $(CRT)

dirs:
	mkdir -p $(BUILD_DIR)

$(CRT): $(CRT_SOURCE)
	$(CC) -m32 -c $< -o $@

$(LIBC): $(OBJECTS)
	$(AR) rcs $@ $^

$(BUILD_DIR)/%.o: ./%.c | dirs
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(LIBC) $(CRT)
