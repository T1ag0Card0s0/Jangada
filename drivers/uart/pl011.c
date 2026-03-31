#include "pl011.h"

static volatile uint32_t *g_uart_dr = 0;
static volatile uint32_t *g_uart_fr = 0;

void pl011_init(uintptr_t base)
{
    g_uart_dr = (volatile uint32_t *)(base + 0x00);
    g_uart_fr = (volatile uint32_t *)(base + 0x18);
}

void pl011_putchar(char c)
{
    while (*g_uart_fr & (1u << 5))
    {
    }
    *g_uart_dr = (uint32_t)c;
}

void pl011_write(const char *data, uint32_t size)
{
    for (uint32_t i = 0; i < size; i++)
    {
        pl011_putchar(data[i]);
    }
}

void pl011_writestring(const char *data)
{
    while (*data)
    {
        pl011_putchar(*data++);
    }
}
