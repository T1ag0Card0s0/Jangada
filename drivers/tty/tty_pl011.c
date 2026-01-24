/**
 * @file tty_pl011.c
 * @brief Glue code to connect PL011 UART driver to TTY layer
 */

#include "drivers/tty/tty_pl011.h"
#include "pl011.h"

/* PL011 operations wrapper functions */

static int pl011_tty_init(void)
{
    return pl011_uart_init();
}

static void pl011_tty_deinit(void)
{
    pl011_uart_deinit();
}

static void pl011_tty_putc(char c)
{
    pl011_uart_putc(c);
}

static char pl011_tty_getc(void)
{
    return pl011_uart_getc();
}

static bool pl011_tty_getc_nonblocking(char *c)
{
    return pl011_uart_getc_nonblocking(c);
}

static size_t pl011_tty_write(const void *data, size_t len)
{
    return pl011_uart_write(data, len);
}

static size_t pl011_tty_read(void *buffer, size_t len)
{
    return pl011_uart_read(buffer, len);
}

static bool pl011_tty_tx_ready(void)
{
    return !pl011_uart_tx_full();
}

static bool pl011_tty_rx_available(void)
{
    return !pl011_uart_rx_empty();
}

static void pl011_tty_flush(void)
{
    pl011_uart_flush();
}

/* PL011 TTY operations table */
static const tty_ops_t pl011_tty_ops = {
        .init = pl011_tty_init,
        .deinit = pl011_tty_deinit,
        .putc = pl011_tty_putc,
        .getc = pl011_tty_getc,
        .getc_nonblocking = pl011_tty_getc_nonblocking,
        .write = pl011_tty_write,
        .read = pl011_tty_read,
        .tx_ready = pl011_tty_tx_ready,
        .rx_available = pl011_tty_rx_available,
        .flush = pl011_tty_flush,
};

/* Static TTY device instance */
static tty_device_t pl011_tty_device;

/**
 * @brief Initialize PL011 as a TTY device
 *
 * @param name Device name (e.g., "tty0", "console")
 * @param line_buffer_size Size of line buffer (0 to disable)
 * @return 0 on success, negative error code on failure
 */
int tty_pl011_init(const char *name, size_t line_buffer_size)
{
    return tty_register(&pl011_tty_device, name, &pl011_tty_ops,
                        line_buffer_size);
}

/**
 * @brief Get the PL011 TTY device
 *
 * @return Pointer to TTY device
 */
tty_device_t *tty_pl011_get(void)
{
    return &pl011_tty_device;
}
