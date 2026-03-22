#ifndef PL011_H
#define PL011_H

#include "driver.h"
#include <stdbool.h>
#include <stdint.h>

#define PL011_IOCTL_SET_BAUD (0x01)
#define PL011_IOCTL_SET_CONFIG (0x02)
#define PL011_IOCTL_FLUSH (0x03)
#define PL011_IOCTL_TX_FULL (0x04)
#define PL011_IOCTL_RX_EMPTY (0x05)
#define PL011_IOCTL_PRINT_HEX (0x06)

typedef enum
{
    UART_DATA_5BITS = 5,
    UART_DATA_6BITS = 6,
    UART_DATA_7BITS = 7,
    UART_DATA_8BITS = 8,
} uart_data_bits_t;

typedef enum
{
    UART_PARITY_NONE,
    UART_PARITY_ODD,
    UART_PARITY_EVEN,
} uart_parity_t;

typedef enum
{
    UART_STOP_1BIT,
    UART_STOP_2BITS,
} uart_stop_bits_t;

typedef struct
{
    uint32_t baudrate;
    uart_data_bits_t data_bits;
    uart_parity_t parity;
    uart_stop_bits_t stop_bits;
    bool enable_fifo;
} pl011_config_t;

typedef struct
{
    uint64_t value;
    int width;
} pl011_print_hex_arg_t;

typedef struct
{
    uint64_t base_addr;
    uint32_t clock_hz;
    pl011_config_t config;
} pl011_priv_t;

extern const driver_ops_t pl011_ops;

void pl011_register(const char *name, pl011_priv_t *priv);

#endif // PL011_H
