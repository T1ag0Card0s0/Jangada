#include "drivers/tty.h"
#include "pl011.h"

/* Declare glue functions */
extern int tty_pl011_init(const char *name, size_t line_buffer_size);
extern tty_device_t *tty_pl011_get(void);

void start_kernel()
{
    /* Initialize TTY subsystem */
    tty_init();

    /* Register PL011 UART as a TTY device */
    /* Line buffer size of 256 bytes enables canonical mode */
    tty_pl011_init("console", 256);

    /* Open the TTY device (initializes hardware) */
    tty_device_t *console = tty_pl011_get();
    tty_open(console);
    tty_puts(console, "Hello from TTY abstraction!\n");

    while (1)
    {
        ;
    }
}
