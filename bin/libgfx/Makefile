CC = i686-elf-gcc
AR = i686-elf-ar

BUILD_DIR = build
SRC_DIR = src
INCLUDE_DIR = include

SYSROOT = ../../sysroot
LIB_NAME = libgfx.a
LIB = $(SYSROOT)/usr/lib/$(LIB_NAME)

CFLAGS = -ffreestanding -O2 -Wall -Wextra \
         -I$(INCLUDE_DIR) \
         -I$(SYSROOT)/usr/include

ARFLAGS = rcs

SRCS := $(shell find $(SRC_DIR) -name '*.c')
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

.PHONY: all clean install

all: install

$(LIB): $(OBJS) | $(SYSROOT)/usr/lib
	$(AR) $(ARFLAGS) $@ $(OBJS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(SYSROOT)/usr/lib:
	mkdir -p $@

install: $(LIB)
	mkdir -p $(SYSROOT)/usr/include/gfx
	cp $(INCLUDE_DIR)/*.h $(SYSROOT)/usr/include/gfx/

clean:
	rm -rf $(BUILD_DIR)
	rm -f $(LIB)
