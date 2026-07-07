
#include <drivers/serial.h>
#include <cpu/io.h>
#include <cpu/cpu.h>
#include <stdint.h>
#include <fs/fs.h>
#include <mm/alloc.h>
#include <string.h>

static void serial_wait_for_transmit(void)
{
        while ((inb(SERIAL_LSR) & 0x20) == 0)
                ;
}

static bool serial_can_read(void)
{
        return inb(SERIAL_PORT + 5) & 0x01;
}

static void serial_wait_for_input(void)
{
        while (!serial_can_read())
                cpu_pause();
}

void serial_putchar(char c)
{
        serial_wait_for_transmit();
        outb(SERIAL_PORT, c);
}

char serial_getchar(void)
{
        serial_wait_for_input();
        return inb(SERIAL_PORT);
}

static int serial_read(file_t *f, void *buf, size_t count)
{
        (void)f;
        char *cbuf = buf;
        for (size_t i = 0; i < count; i++)
        {
                cbuf[i] = serial_getchar();
        }
        return count;
}

static int serial_write(file_t *f, const void *buf, size_t count)
{
        (void)f;
        const char *cbuf = buf;
        for (size_t i = 0; i < count; i++)
        {
                serial_putchar(cbuf[i]);
        }
        return count;
}

static file_ops_t serial_ops =
{
        .read = serial_read,
        .write = serial_write,
        .open = NULL,
        .close = NULL,
        .lseek = NULL,
};

void serial_register_device(void)
{
        vnode_t *serial = vfs_create("/dev", "serial", 0);
        serial->ops = &serial_ops;
}

void serial_init(void)
{
        outb(SERIAL_IER, 0x00);  // Disable all interrupts
        outb(SERIAL_LCR, 0x80);  // Set DLAB (Data Access Bit) to 1
        outb(SERIAL_PORT, 0x03);  // Set LSB of baud rate divisor (e.g., 0x03 for 115200 baud)
        outb(SERIAL_IER, 0x00);  // Disable interrupts again (no need to enable them yet)
        outb(SERIAL_LCR, 0x03);  // LCR = 0x03 (8 bits, no parity, 1 stop bit)
        outb(SERIAL_PORT + 2, 0xC7);  // FIFO Control Register: Enable FIFO, clear RX/TX
        outb(SERIAL_MCR, 0x0B);  // Enable DTR and RTS for flow control
        serial_register_device();
}
