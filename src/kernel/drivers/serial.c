#include <drivers/serial.h>
#include <io.h>
#include <arch.h>

void serial_init(void)
{
        outb(SERIAL_IER, 0x00);  // Disable all interrupts
        outb(SERIAL_LCR, 0x80);  // Set DLAB (Data Access Bit) to 1
        outb(SERIAL_PORT, 0x03);  // Set LSB of baud rate divisor (e.g., 0x03 for 115200 baud)
        outb(SERIAL_IER, 0x00);  // Disable interrupts again (no need to enable them yet)
        outb(SERIAL_LCR, 0x03);  // LCR = 0x03 (8 bits, no parity, 1 stop bit)
        outb(SERIAL_PORT + 2, 0xC7);  // FIFO Control Register: Enable FIFO, clear RX/TX
        outb(SERIAL_MCR, 0x0B);  // Enable DTR and RTS for flow control
}

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
                pause();
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
