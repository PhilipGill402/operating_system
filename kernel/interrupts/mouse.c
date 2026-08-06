#include "interrupts/mouse.h"

static mouse_state_t mouse = { 0 };
static uint8_t mouse_packet[3];
static uint8_t mouse_cycle = 0;

static void ps2_wait_read(void) {
    while (!(arch_read_byte(PS2_STATUS_PORT) & PS2_STATUS_OUTPUT_FULL)) {
        asm volatile("pause");
    }
}

static void ps2_wait_write(void) {
    while (arch_read_byte(PS2_STATUS_PORT) & PS2_STATUS_INPUT_FULL) {
        asm volatile("pause");
    }
}

static uint8_t ps2_read_data(void) {
    ps2_wait_read();
    return arch_read_byte(PS2_DATA_PORT);
}

static void ps2_write_command(uint8_t cmd) {
    ps2_wait_write();
    arch_write_byte(PS2_COMMAND_PORT, cmd);
}

static void ps2_write_data(uint8_t data) {
    ps2_wait_write();
    arch_write_byte(PS2_DATA_PORT, data);
}

static void mouse_write(uint8_t data) {
    ps2_write_command(PS2_CMD_WRITE_MOUSE);
    ps2_write_data(data);
}

static uint8_t mouse_read(void) {
    return ps2_read_data();
}

void mouse_irq_handler(arch_trapframe_t* tf) {
    (void)tf;

    uint8_t status = arch_read_byte(PS2_STATUS_PORT);
    if (!(status & PS2_STATUS_OUTPUT_FULL)) {
        return;
    }

    uint8_t data = arch_read_byte(PS2_DATA_PORT);
    
    // the third bit is always set on the first packet
    if (mouse_cycle == 0 && !(data & 0x08))
        return;

    mouse_packet[mouse_cycle++] = data;
    
    // we are waiting on more packets
    if (mouse_cycle < 3)
        return;
    
    mouse_cycle = 0;

    uint8_t b0 = mouse_packet[0];
    uint8_t b1 = mouse_packet[1];
    uint8_t b2 = mouse_packet[2];
    
    // ignore overflow packets
    if (b0 & 0xC0) {
        return;
    }
    
    mouse.left_clicked = (b0 & 1) > 0; // checks first bit
    mouse.right_clicked = (b0 & 2) > 0; // checks second bit
    mouse.middle_clicked = (b0 & 4) > 0; // checks third bit
    
    int32_t dx = (int8_t)b1;
    int32_t dy = (int8_t)b2;

    mouse.prev_x = mouse.x;
    mouse.prev_y = mouse.y;

    mouse.x += dx;
    mouse.y -= dy;

    if (mouse.x >= framebuffer.width)
        mouse.x = framebuffer.width - 1;
    if (mouse.x < 0)
        mouse.x = 0;
    if (mouse.y >= framebuffer.height)
        mouse.y = framebuffer.height - 1;
    if (mouse.y < 0)
        mouse.y = 0;

    memset(&mouse_packet, 0, sizeof(mouse_packet));

    input_event_t event;
    
    if (mouse.left_clicked || mouse.right_clicked || mouse.middle_clicked)
        event.type = INPUT_EVENT_MOUSE_CLICK;
    else
        event.type = INPUT_EVENT_MOUSE;
    
    if (mouse.left_clicked)
        event.mouse_buttons |= MOUSE_BUTTON_LEFT;
    else if (mouse.right_clicked)
        event.mouse_buttons |= MOUSE_BUTTON_RIGHT;
    else if (mouse.middle_clicked)
        event.mouse_buttons |= MOUSE_BUTTON_CENTER;
    
    event.mouse_x = mouse.x;
    event.mouse_y = mouse.y;
    event.mouse_dx = dx;
    event.mouse_dy = dy;

    enqueue(&input_buffer, &event);
}

void mouse_init() {
    ps2_write_command(PS2_CMD_ENABLE_AUX);

    ps2_write_command(PS2_CMD_READ_CONFIG);
    uint8_t config = ps2_read_data();

    config |= 0x02;
    config &= ~0x20;

    ps2_write_command(PS2_CMD_WRITE_CONFIG);
    ps2_write_data(config);

    mouse_write(MOUSE_CMD_SET_DEFAULTS);
    uint8_t ack = mouse_read();

    if (ack != MOUSE_ACK) {
        log_error("mouse: set defaults failed ack=%x\n", ack);
    }

    mouse_write(MOUSE_CMD_ENABLE);
    ack = mouse_read();

    if (ack != MOUSE_ACK) {
        log_error("mouse: enable failed ack=%x\n", ack);
    }

    irq_install_handler(12, mouse_irq_handler);

    pic_clear_mask(12);

    log_debug("mouse initialized\n");
}



