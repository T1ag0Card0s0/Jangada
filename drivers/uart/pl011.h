#ifndef DRIVERS_UART_PL011_H
#define DRIVERS_UART_PL011_H

#include <stdint.h>

void pl011_init(uintptr_t base);
void pl011_putchar(char c);
void pl011_write(const char *data, uint32_t size);
void pl011_writestring(const char *data);

#endif /* DRIVERS_UART_PL011_H */
