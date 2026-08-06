#include <arch/interrupts/pic.h>

void pic_remap(uint8_t offset1, uint8_t offset2) {
    uint8_t mask1 = arch_read_byte(PIC1_DATA);
    uint8_t mask2 = arch_read_byte(PIC2_DATA);

    arch_write_byte(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    arch_io_wait();
    arch_write_byte(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    arch_io_wait();

    arch_write_byte(PIC1_DATA, offset1);
    arch_io_wait();
    arch_write_byte(PIC2_DATA, offset2);
    arch_io_wait();

    arch_write_byte(PIC1_DATA, 0x04); // slave PIC is on IRQ2
    arch_io_wait();
    arch_write_byte(PIC2_DATA, 0x02); // slave identity is 2
    arch_io_wait();

    arch_write_byte(PIC1_DATA, ICW4_8086);
    arch_io_wait();
    arch_write_byte(PIC2_DATA, ICW4_8086);
    arch_io_wait();

    arch_write_byte(PIC1_DATA, mask1);
    arch_write_byte(PIC2_DATA, mask2);
}

void pic_set_mask(uint8_t irq_line) {
    uint16_t port;
    uint8_t value;

    if (irq_line < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq_line -= 8;
    }

    value = arch_read_byte(port) | (1 << irq_line);
    arch_write_byte(port, value);
}

void pic_clear_mask(uint8_t irq_line) {
    uint16_t port;
    uint8_t value;

    if (irq_line < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq_line -= 8;
    }

    value = arch_read_byte(port) & ~(1 << irq_line);
    arch_write_byte(port, value);
}

void pic_send_eoi(uint8_t irq_line) {
    if (irq_line >= 8) {
        arch_write_byte(PIC2_COMMAND, 0x20);
    }
    arch_write_byte(PIC1_COMMAND, 0x20);
}
