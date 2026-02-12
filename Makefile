BUILD_DIR=build
BOOT_OBJS=$(wildcard $(BUILD_DIR)/bootloader/*.o)
KERNEL_OBJS=$(wildcard $(BUILD_DIR)/os/*.o)
BOOTLOADER=$(BUILD_DIR)/bootloader/bootloader.o
KERNEL=$(BUILD_DIR)/os/kernel.o
LINKER=linker.ld
KERNEL_LINKER=kernel.ld
DISK_IMG=$(BUILD_DIR)/disk.img

KERNEL_ELF=$(BUILD_DIR)/os/kernel.elf
BOOT_BIN=$(BUILD_DIR)/bootloader/bootloader.bin
KERNEL_BIN=$(BUILD_DIR)/os/kernel.bin

all: bootdisk

.PHONY: bootloader os

$(BOOT_BIN):
	make -C bootloader

$(KERNEL_BIN):
	make -C os

bootdisk: $(BOOT_BIN) $(KERNEL_BIN) 
	dd if=/dev/zero of=$(DISK_IMG) bs=512 count=2880
	dd conv=notrunc if=$(BOOT_BIN) of=$(DISK_IMG) bs=512 count=1 seek=0
	dd conv=notrunc if=$(KERNEL_BIN) of=$(DISK_IMG) bs=512 seek=1

qemu-debug:
	qemu-system-i386 -machine q35 -fda $(DISK_IMG) -gdb tcp::26000 -S

qemu:
	qemu-system-i386 -fda $(DISK_IMG)


clean:
	make -C bootloader clean
	make -C os clean
	rm $(DISK_IMG)
