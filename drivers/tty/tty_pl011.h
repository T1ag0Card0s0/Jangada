#ifndef TTY_PL011_H
#define TTY_PL011_H

#include "drivers/tty.h"
#include <stddef.h>

/* Declare glue functions */
int tty_pl011_init(const char *name, size_t line_buffer_size);
tty_device_t *tty_pl011_get(void);

#endif // TTY_PL011_H
