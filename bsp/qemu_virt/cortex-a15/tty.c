#include "../../../drivers/uart/pl011.h"
#include "board.h"
#include <jangada/tty.h>
#include <stddef.h>

void terminal_initialize(void)
{
    pl011_init(QEMU_VIRT_UART0_BASE);
}

void terminal_putchar(char c)
{
    pl011_putchar(c);
}

void terminal_write(const char *data, size_t size)
{
    pl011_write(data, (uint32_t)size);
}

void terminal_writestring(const char *data)
{
    pl011_writestring(data);
}
