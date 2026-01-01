/**
 * @file tty.c
 * @brief TTY abstraction layer implementation
 */

#include "drivers/tty.h"
#include <stdarg.h>
#include <string.h>

/* Configuration */
#ifndef TTY_MAX_DEVICES
#define TTY_MAX_DEVICES 4
#endif

/* Error codes */
#define TTY_ERR_INVALID (-1)
#define TTY_ERR_NO_SLOTS (-2)
#define TTY_ERR_NO_MEM (-3)
#define TTY_ERR_NOT_FOUND (-4)

/* Internal state */
static tty_device_t *tty_devices[TTY_MAX_DEVICES];
static int tty_device_count = 0;

/* Global console pointer */
tty_device_t *tty_console = NULL;

/* Optional: Thread safety stubs - replace with your RTOS primitives */
static inline void tty_lock(tty_device_t *tty)
{
    /* TODO: Implement mutex lock if tty->mutex is set */
    (void)tty;
}

static inline void tty_unlock(tty_device_t *tty)
{
    /* TODO: Implement mutex unlock if tty->mutex is set */
    (void)tty;
}

/* Internal helpers */
static void *tty_malloc(size_t size)
{
    /** TODO: provide kmalloc */
    return NULL;
}

static void tty_free(void *ptr)
{
    /* TODO: kfree */
}

/* Public API Implementation */

int tty_init(void)
{
    tty_device_count = 0;
    for (int i = 0; i < TTY_MAX_DEVICES; i++)
    {
        tty_devices[i] = NULL;
    }
    return 0;
}

int tty_register(tty_device_t *tty, const char *name, const tty_ops_t *ops,
                 size_t line_buffer_size)
{
    if (!tty || !name || !ops)
    {
        return TTY_ERR_INVALID;
    }

    if (tty_device_count >= TTY_MAX_DEVICES)
    {
        return TTY_ERR_NO_SLOTS;
    }

    /* Initialize TTY structure */
    memset(tty, 0, sizeof(tty_device_t));
    tty->name = name;
    tty->ops = ops;
    tty->flags = TTY_FLAG_CRLF; /* Default: convert LF to CRLF */

    /* Allocate line buffer if requested */
    if (line_buffer_size > 0)
    {
        tty->line_buffer = tty_malloc(line_buffer_size);
        if (!tty->line_buffer)
        {
            return TTY_ERR_NO_MEM;
        }
        tty->line_buffer_size = line_buffer_size;
        tty->line_buffer_pos = 0;
        tty->flags |= TTY_FLAG_CANONICAL;
    }

    /* Add to device list */
    tty_devices[tty_device_count++] = tty;

    /* Set as default console if first device */
    if (!tty_console)
    {
        tty_console = tty;
    }

    return 0;
}

void tty_unregister(tty_device_t *tty)
{
    if (!tty)
    {
        return;
    }

    /* Remove from device list */
    for (int i = 0; i < tty_device_count; i++)
    {
        if (tty_devices[i] == tty)
        {
            /* Shift remaining devices */
            for (int j = i; j < tty_device_count - 1; j++)
            {
                tty_devices[j] = tty_devices[j + 1];
            }
            tty_device_count--;
            break;
        }
    }

    /* Free line buffer */
    if (tty->line_buffer)
    {
        tty_free(tty->line_buffer);
    }

    /* Clear console if this was it */
    if (tty_console == tty)
    {
        tty_console = tty_device_count > 0 ? tty_devices[0] : NULL;
    }
}

tty_device_t *tty_get(const char *name)
{
    if (!name)
    {
        return NULL;
    }

    for (int i = 0; i < tty_device_count; i++)
    {
        if (strcmp(tty_devices[i]->name, name) == 0)
        {
            return tty_devices[i];
        }
    }

    return NULL;
}

int tty_open(tty_device_t *tty)
{
    if (!tty || !tty->ops || !tty->ops->init)
    {
        return TTY_ERR_INVALID;
    }

    return tty->ops->init();
}

void tty_close(tty_device_t *tty)
{
    if (!tty || !tty->ops)
    {
        return;
    }

    if (tty->ops->deinit)
    {
        tty->ops->deinit();
    }
}

void tty_putc(tty_device_t *tty, char c)
{
    if (!tty || !tty->ops || !tty->ops->putc)
    {
        return;
    }

    tty_lock(tty);

    /* Handle CRLF conversion */
    if ((tty->flags & TTY_FLAG_CRLF) && c == '\n')
    {
        tty->ops->putc('\r');
        tty->bytes_written++;
    }

    tty->ops->putc(c);
    tty->bytes_written++;

    tty_unlock(tty);
}

void tty_puts(tty_device_t *tty, const char *str)
{
    if (!tty || !str)
    {
        return;
    }

    while (*str)
    {
        tty_putc(tty, *str++);
    }
}

size_t tty_write(tty_device_t *tty, const void *data, size_t len)
{
    if (!tty || !data)
    {
        return 0;
    }

    const char *bytes = (const char *)data;
    size_t written = 0;

    tty_lock(tty);

    /* Use hardware write if available and no special processing needed */
    if (tty->ops->write && !(tty->flags & TTY_FLAG_CRLF))
    {
        written = tty->ops->write(data, len);
        tty->bytes_written += written;
    }
    else
    {
        /* Character-by-character with processing */
        for (size_t i = 0; i < len; i++)
        {
            tty_putc(tty, bytes[i]);
            written++;
        }
    }

    tty_unlock(tty);
    return written;
}

char tty_getc(tty_device_t *tty)
{
    if (!tty || !tty->ops || !tty->ops->getc)
    {
        return 0;
    }

    tty_lock(tty);

    char c = tty->ops->getc();
    tty->bytes_read++;

    /* Handle CR to LF conversion */
    if ((tty->flags & TTY_FLAG_CRLFNL) && c == '\r')
    {
        c = '\n';
    }

    /* Echo if enabled */
    if (tty->flags & TTY_FLAG_ECHO)
    {
        tty_putc(tty, c);
    }

    tty_unlock(tty);
    return c;
}

bool tty_getc_nonblocking(tty_device_t *tty, char *c)
{
    if (!tty || !c || !tty->ops || !tty->ops->getc_nonblocking)
    {
        return false;
    }

    tty_lock(tty);

    bool result = tty->ops->getc_nonblocking(c);

    if (result)
    {
        tty->bytes_read++;

        /* Handle CR to LF conversion */
        if ((tty->flags & TTY_FLAG_CRLFNL) && *c == '\r')
        {
            *c = '\n';
        }

        /* Echo if enabled */
        if (tty->flags & TTY_FLAG_ECHO)
        {
            tty_putc(tty, *c);
        }
    }

    tty_unlock(tty);
    return result;
}

size_t tty_readline(tty_device_t *tty, char *buffer, size_t size)
{
    if (!tty || !buffer || size == 0)
    {
        return 0;
    }

    if (!(tty->flags & TTY_FLAG_CANONICAL))
    {
        /* Non-canonical mode: just read available data */
        return tty_read(tty, buffer, size);
    }

    size_t pos = 0;

    tty_lock(tty);

    while (pos < size - 1)
    {
        char c = tty_getc(tty);

        /* Handle backspace */
        if (c == '\b' || c == 0x7F)
        {
            if (pos > 0)
            {
                pos--;
                tty_puts(tty, "\b \b"); /* Erase character on screen */
            }
            continue;
        }

        /* Handle end of line */
        if (c == '\n' || c == '\r')
        {
            buffer[pos] = '\0';
            tty_putc(tty, '\n');
            break;
        }

        buffer[pos++] = c;
    }

    buffer[pos] = '\0';

    tty_unlock(tty);
    return pos;
}

size_t tty_read(tty_device_t *tty, void *buffer, size_t len)
{
    if (!tty || !buffer || len == 0)
    {
        return 0;
    }

    tty_lock(tty);

    size_t read = 0;

    if (tty->ops->read)
    {
        read = tty->ops->read(buffer, len);
        tty->bytes_read += read;
    }
    else if (tty->ops->getc_nonblocking)
    {
        char *bytes = (char *)buffer;
        for (size_t i = 0; i < len; i++)
        {
            if (!tty->ops->getc_nonblocking(&bytes[i]))
            {
                break;
            }
            read++;
        }
        tty->bytes_read += read;
    }

    tty_unlock(tty);
    return read;
}

void tty_flush(tty_device_t *tty)
{
    if (!tty || !tty->ops)
    {
        return;
    }

    tty_lock(tty);

    if (tty->ops->flush)
    {
        tty->ops->flush();
    }

    tty_unlock(tty);
}

void tty_set_flags(tty_device_t *tty, uint32_t flags)
{
    if (!tty)
    {
        return;
    }

    tty_lock(tty);
    tty->flags |= flags;
    tty_unlock(tty);
}

void tty_clear_flags(tty_device_t *tty, uint32_t flags)
{
    if (!tty)
    {
        return;
    }

    tty_lock(tty);
    tty->flags &= ~flags;
    tty_unlock(tty);
}

uint32_t tty_get_flags(tty_device_t *tty)
{
    if (!tty)
    {
        return 0;
    }

    return tty->flags;
}

/* Simple printf implementation */
static void tty_print_dec(tty_device_t *tty, int64_t value)
{
    if (value < 0)
    {
        tty_putc(tty, '-');
        value = -value;
    }

    char buffer[20];
    int pos = 0;

    do
    {
        buffer[pos++] = '0' + (value % 10);
        value /= 10;
    } while (value > 0);

    while (pos > 0)
    {
        tty_putc(tty, buffer[--pos]);
    }
}

static void tty_print_uint(tty_device_t *tty, uint64_t value)
{
    char buffer[20];
    int pos = 0;

    do
    {
        buffer[pos++] = '0' + (value % 10);
        value /= 10;
    } while (value > 0);

    while (pos > 0)
    {
        tty_putc(tty, buffer[--pos]);
    }
}

void tty_print_hex(tty_device_t *tty, uint64_t value, int width)
{
    if (!tty)
    {
        return;
    }

    static const char hex[] = "0123456789ABCDEF";

    tty_puts(tty, "0x");

    if (width <= 0 || width > 64)
    {
        width = 64;
    }

    for (int i = width - 4; i >= 0; i -= 4)
    {
        tty_putc(tty, hex[(value >> i) & 0xF]);
    }
}

int tty_printf(tty_device_t *tty, const char *fmt, ...)
{
    if (!tty || !fmt)
    {
        return 0;
    }

    va_list args;
    va_start(args, fmt);

    int count = 0;
    const char *p = fmt;

    tty_lock(tty);

    while (*p)
    {
        if (*p == '%')
        {
            p++;
            switch (*p)
            {
                case 'd':
                case 'i':
                {
                    int val = va_arg(args, int);
                    tty_print_dec(tty, val);
                    break;
                }
                case 'u':
                {
                    unsigned int val = va_arg(args, unsigned int);
                    tty_print_uint(tty, val);
                    break;
                }
                case 'x':
                case 'X':
                {
                    unsigned int val = va_arg(args, unsigned int);
                    tty_print_hex(tty, val, 32);
                    break;
                }
                case 'p':
                {
                    void *val = va_arg(args, void *);
                    tty_print_hex(tty, (uint64_t)val, 64);
                    break;
                }
                case 's':
                {
                    const char *str = va_arg(args, const char *);
                    if (str)
                    {
                        tty_puts(tty, str);
                    }
                    else
                    {
                        tty_puts(tty, "(null)");
                    }
                    break;
                }
                case 'c':
                {
                    char c = (char)va_arg(args, int);
                    tty_putc(tty, c);
                    break;
                }
                case '%':
                    tty_putc(tty, '%');
                    break;
                default:
                    tty_putc(tty, '%');
                    tty_putc(tty, *p);
                    break;
            }
            p++;
        }
        else
        {
            tty_putc(tty, *p++);
        }
        count++;
    }

    tty_unlock(tty);

    va_end(args);
    return count;
}

bool tty_rx_available(tty_device_t *tty)
{
    if (!tty || !tty->ops)
    {
        return false;
    }

    if (tty->ops->rx_available)
    {
        return tty->ops->rx_available();
    }

    return false;
}

bool tty_tx_ready(tty_device_t *tty)
{
    if (!tty || !tty->ops)
    {
        return false;
    }

    if (tty->ops->tx_ready)
    {
        return tty->ops->tx_ready();
    }

    return true; /* Assume ready if not implemented */
}
