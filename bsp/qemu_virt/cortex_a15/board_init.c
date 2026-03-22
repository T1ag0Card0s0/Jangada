#include "pl011.h"
#include "registry.h"

extern const driver_ops_t pl011_ops;

#define QEMU_VIRT_PL011_CLK_HZ 24000000UL

#define PL011_DEFAULT_CFG                                                      \
    {                                                                          \
            .baudrate = 115200,                                                \
            .data_bits = UART_DATA_8BITS,                                      \
            .parity = UART_PARITY_NONE,                                        \
            .stop_bits = UART_STOP_1BIT,                                       \
            .enable_fifo = true,                                               \
    }

static pl011_priv_t uart0_priv = {
        .base_addr = 0x09000000UL,
        .clock_hz = QEMU_VIRT_PL011_CLK_HZ,
        .config = PL011_DEFAULT_CFG,
};

static device_t uart0 = {
        .name = "uart0", .ops = &pl011_ops, .priv = &uart0_priv};

void board_init(void)
{
    device_register(&uart0);
    device_open(&uart0);
}
