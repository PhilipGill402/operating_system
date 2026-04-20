ARCH := i686
TARGET := $(ARCH)-elf

CC := $(TARGET)-gcc

SYSROOT := ../sysroot
OUTDIR := ../fs/usr

CFLAGS := -std=gnu99 -ffreestanding -O2 -Wall -Wextra -m32 -I$(SYSROOT)/usr/include
LDFLAGS := -nostdlib -nostartfiles -ffreestanding -m32

CRT0 := $(SYSROOT)/usr/lib/crt0.o
LIBC := $(SYSROOT)/usr/lib/libc.a

C_SOURCES := $(wildcard *.c)
PROGS := $(patsubst %.c,$(OUTDIR)/%.elf,$(C_SOURCES))

.PHONY: all clean

all: $(PROGS)

$(OUTDIR)/%.elf: %.c $(CRT0) $(LIBC)
	mkdir -p $(OUTDIR)
	$(CC) $(CFLAGS) $(LDFLAGS) $< $(CRT0) $(LIBC) -o $@

clean:
	rm -f $(OUTDIR)/*.elf
