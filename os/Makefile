BUILD_DIR=../build/os

CFLAGS+=-ffreestanding -fno-pie -fno-pic -fno-stack-protector -nostdlib

OS_SRCS := $(wildcard *.c)
OS_OBJS := $(patsubst %.c, $(BUILD_DIR)/%.o, $(OS_SRCS))

all: $(OS_OBJS)

$(BUILD_DIR)/%.o: %.c
	gcc $(CFLAGS) -m32 -c  $< -o $@

clean:
	rm $(BUILD_DIR)/*



