#ifndef SERIAL_H
#define SERIAL_H

#define SERIAL_PORT 0x3F8
#define SERIAL_IER  (SERIAL_PORT + 1)  // Interrupt Enable Register
#define SERIAL_LCR  (SERIAL_PORT + 3)  // Line Control Register
#define SERIAL_LSR  (SERIAL_PORT + 5)  // Line Status Register
#define SERIAL_MCR  (SERIAL_PORT + 4)  // Modem Control Register

void serial_init(void);
char serial_getchar(void);
void serial_putchar(char c);

#endif
