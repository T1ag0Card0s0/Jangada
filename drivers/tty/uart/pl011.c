/**
 * @file pl011.c
 * @brief PL011 UART driver implementation
 */

#include "pl011.h"
#include <stddef.h>

/* PL011 Register Offsets */
#define UART_DR 0x00    /* Data Register */
#define UART_RSR 0x04   /* Receive Status Register */
#define UART_FR 0x18    /* Flag Register */
#define UART_ILPR 0x20  /* IrDA Low-Power Counter Register */
#define UART_IBRD 0x24  /* Integer Baud Rate Divisor */
#define UART_FBRD 0x28  /* Fractional Baud Rate Divisor */
#define UART_LCRH 0x2C  /* Line Control Register */
#define UART_CR 0x30    /* Control Register */
#define UART_IFLS 0x34  /* Interrupt FIFO Level Select */
#define UART_IMSC 0x38  /* Interrupt Mask Set/Clear */
#define UART_RIS 0x3C   /* Raw Interrupt Status */
#define UART_MIS 0x40   /* Masked Interrupt Status */
#define UART_ICR 0x44   /* Interrupt Clear Register */
#define UART_DMACR 0x48 /* DMA Control Register */

/* Flag Register bits */
#define FR_TXFE (1u << 7) /* Transmit FIFO empty */
#define FR_RXFF (1u << 6) /* Receive FIFO full */
#define FR_TXFF (1u << 5) /* Transmit FIFO full */
#define FR_RXFE (1u << 4) /* Receive FIFO empty */
#define FR_BUSY (1u << 3) /* UART busy */

/* Line Control Register bits */
#define LCRH_SPS (1u << 7)    /* Stick parity select */
#define LCRH_WLEN_8 (3u << 5) /* 8 bits */
#define LCRH_WLEN_7 (2u << 5) /* 7 bits */
#define LCRH_WLEN_6 (1u << 5) /* 6 bits */
#define LCRH_WLEN_5 (0u << 5) /* 5 bits */
#define LCRH_FEN (1u << 4)    /* Enable FIFOs */
#define LCRH_STP2 (1u << 3)   /* Two stop bits */
#define LCRH_EPS (1u << 2)    /* Even parity select */
#define LCRH_PEN (1u << 1)    /* Parity enable */
#define LCRH_BRK (1u << 0)    /* Send break */

/* Control Register bits */
#define CR_CTSEN (1u << 15) /* CTS hardware flow control enable */
#define CR_RTSEN (1u << 14) /* RTS hardware flow control enable */
#define CR_RTS (1u << 11)   /* Request to send */
#define CR_DTR (1u << 10)   /* Data transmit ready */
#define CR_RXE (1u << 9)    /* Receive enable */
#define CR_TXE (1u << 8)    /* Transmit enable */
#define CR_LBE (1u << 7)    /* Loopback enable */
#define CR_UARTEN (1u << 0) /* UART enable */

/* Error codes */
#define UART_ERR_INVALID_CONFIG (-1)

/* Static helpers */
static inline void mmio_write(uint64_t reg, uint32_t val)
{
    *(volatile uint32_t *)(reg) = val;
}

static inline uint32_t mmio_read(uint64_t reg)
{
    return *(volatile uint32_t *)(reg);
}

static inline uint64_t UART_REG(uint32_t offset)
{
    return (UART0_BASE_ADDR + offset);
}

/* Default UART configuration */
static uart_config_t default_config = {.baudrate = UART0_BAUDRATE,
                                       .data_bits = UART_DATA_8BITS,
                                       .parity = UART_PARITY_NONE,
                                       .stop_bits = UART_STOP_1BIT,
                                       .enable_fifo = true};

/* Public API Implementation */

int pl011_uart_init(void)
{
    return pl011_uart_init_config(&default_config);
}

int pl011_uart_init_config(const uart_config_t *config)
{
    if (!config)
    {
        return UART_ERR_INVALID_CONFIG;
    }

    /* Disable UART */
    mmio_write(UART_REG(UART_CR), 0);

    /* Clear all interrupt status */
    mmio_write(UART_REG(UART_ICR), 0x7FF);

    /* Calculate baud rate divisors */
    /* Baud rate divisor = UARTCLK / (16 × baud rate) */
    /* IBRD = integer part, FBRD = fractional part × 64 + 0.5 */
    uint32_t divisor = (UART0_CLOCK_HZ * 4) / config->baudrate;
    uint32_t ibrd = divisor / 64;
    uint32_t fbrd = divisor % 64;

    mmio_write(UART_REG(UART_IBRD), ibrd);
    mmio_write(UART_REG(UART_FBRD), fbrd);

    /* Configure line control */
    uint32_t lcrh = 0;

    /* Data bits */
    switch (config->data_bits)
    {
        case UART_DATA_5BITS:
            lcrh |= LCRH_WLEN_5;
            break;
        case UART_DATA_6BITS:
            lcrh |= LCRH_WLEN_6;
            break;
        case UART_DATA_7BITS:
            lcrh |= LCRH_WLEN_7;
            break;
        case UART_DATA_8BITS:
            lcrh |= LCRH_WLEN_8;
            break;
    }

    /* Parity */
    if (config->parity != UART_PARITY_NONE)
    {
        lcrh |= LCRH_PEN;
        if (config->parity == UART_PARITY_EVEN)
        {
            lcrh |= LCRH_EPS;
        }
    }

    /* Stop bits */
    if (config->stop_bits == UART_STOP_2BITS)
    {
        lcrh |= LCRH_STP2;
    }

    /* FIFO */
    if (config->enable_fifo)
    {
        lcrh |= LCRH_FEN;
    }

    mmio_write(UART_REG(UART_LCRH), lcrh);

    /* Enable UART, TX, and RX */
    mmio_write(UART_REG(UART_CR), CR_UARTEN | CR_TXE | CR_RXE);

    return 0;
}

void pl011_uart_deinit(void)
{
    /* Wait for transmission to complete */
    pl011_uart_flush();

    /* Disable UART */
    mmio_write(UART_REG(UART_CR), 0);
}

void pl011_uart_putc(char c)
{
    /* Handle newline conversion */
    if (c == '\n')
    {
        pl011_uart_putc('\r');
    }

    /* Wait until TX FIFO has space */
    while (mmio_read(UART_REG(UART_FR)) & FR_TXFF)
    {
        /* Busy wait */
    }

    mmio_write(UART_REG(UART_DR), (uint32_t)c);
}

void pl011_uart_puts(const char *str)
{
    if (!str)
    {
        return;
    }

    while (*str)
    {
        pl011_uart_putc(*str++);
    }
}

size_t pl011_uart_write(const void *data, size_t len)
{
    if (!data)
    {
        return 0;
    }

    const char *bytes = (const char *)data;
    for (size_t i = 0; i < len; i++)
    {
        pl011_uart_putc(bytes[i]);
    }

    return len;
}

char pl011_uart_getc(void)
{
    /* Wait until RX FIFO has data */
    while (mmio_read(UART_REG(UART_FR)) & FR_RXFE)
    {
        /* Busy wait */
    }

    return (char)(mmio_read(UART_REG(UART_DR)) & 0xFF);
}

bool pl011_uart_getc_nonblocking(char *c)
{
    if (!c)
    {
        return false;
    }

    /* Check if RX FIFO has data */
    if (mmio_read(UART_REG(UART_FR)) & FR_RXFE)
    {
        return false;
    }

    *c = (char)(mmio_read(UART_REG(UART_DR)) & 0xFF);
    return true;
}

size_t pl011_uart_read(void *buffer, size_t len)
{
    if (!buffer || len == 0)
    {
        return 0;
    }

    char *bytes = (char *)buffer;
    size_t count = 0;

    while (count < len)
    {
        if (!pl011_uart_getc_nonblocking(&bytes[count]))
        {
            break;
        }
        count++;
    }

    return count;
}

bool pl011_uart_tx_full(void)
{
    return (mmio_read(UART_REG(UART_FR)) & FR_TXFF) != 0;
}

bool pl011_uart_rx_empty(void)
{
    return (mmio_read(UART_REG(UART_FR)) & FR_RXFE) != 0;
}

void pl011_uart_flush(void)
{
    /* Wait until TX FIFO is empty and UART is not busy */
    while (mmio_read(UART_REG(UART_FR)) & (FR_BUSY))
    {
        /* Busy wait */
    }
}

void pl011_uart_print_hex(uint64_t value, int width)
{
    static const char hex[] = "0123456789ABCDEF";

    pl011_uart_puts("0x");

    if (width <= 0 || width > 64)
    {
        width = 64;
    }

    for (int i = width - 4; i >= 0; i -= 4)
    {
        pl011_uart_putc(hex[(value >> i) & 0xF]);
    }
}
