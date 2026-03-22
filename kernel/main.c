#include "drivers/tty/tty.h"
#include "drivers/tty/tty_pl011.h"

void start_kernel()
{
    /* Initialize TTY subsystem */
    tty_init();

    /* Register PL011 UART as a TTY device */
    /* Line buffer disabled (0) since kmalloc not yet implemented */
    tty_pl011_init("console", 0);

    /* Open the TTY device (initializes hardware) */
    tty_device_t *console = tty_pl011_get();
    tty_open(console);
    tty_puts(console, "Hello from TTY abstraction!\n");

    while (1)
    {
        ;
    }
}
