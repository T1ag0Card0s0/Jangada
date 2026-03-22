#include "pl011_driver.h"
#include <stddef.h>

#define UART_DR 0x00
#define UART_RSR 0x04
#define UART_FR 0x18
#define UART_IBRD 0x24
#define UART_FBRD 0x28
#define UART_LCRH 0x2C
#define UART_CR 0x30
#define UART_IMSC 0x38
#define UART_ICR 0x44

#define FR_TXFF (1u << 5)
#define FR_RXFE (1u << 4)
#define FR_BUSY (1u << 3)

#define LCRH_WLEN_8 (3u << 5)
#define LCRH_WLEN_7 (2u << 5)
#define LCRH_WLEN_6 (1u << 5)
#define LCRH_WLEN_5 (0u << 5)
#define LCRH_FEN (1u << 4)
#define LCRH_STP2 (1u << 3)
#define LCRH_EPS (1u << 2)
#define LCRH_PEN (1u << 1)

#define CR_RXE (1u << 9)
#define CR_TXE (1u << 8)
#define CR_UARTEN (1u << 0)

static inline void reg_write(uint64_t base, uint32_t off, uint32_t val)
{
    *(volatile uint32_t *)(base + off) = val;
}

static inline uint32_t reg_read(uint64_t base, uint32_t off)
{
    return *(volatile uint32_t *)(base + off);
}

static int pl011_do_config(pl011_priv_t *p)
{
    const pl011_config_t *cfg = &p->config;

    reg_write(p->base_addr, UART_CR, 0);

    reg_write(p->base_addr, UART_ICR, 0x7FF);

    uint32_t divisor = (p->clock_hz * 4) / cfg->baudrate;
    reg_write(p->base_addr, UART_IBRD, divisor / 64);
    reg_write(p->base_addr, UART_FBRD, divisor % 64);

    uint32_t lcrh = 0;
    switch (cfg->data_bits)
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
        default:
            lcrh |= LCRH_WLEN_8;
            break;
    }
    if (cfg->parity != UART_PARITY_NONE)
    {
        lcrh |= LCRH_PEN;
        if (cfg->parity == UART_PARITY_EVEN)
        {
            lcrh |= LCRH_EPS;
        }
    }
    if (cfg->stop_bits == UART_STOP_2BITS)
    {
        lcrh |= LCRH_STP2;
    }
    if (cfg->enable_fifo)
    {
        lcrh |= LCRH_FEN;
    }

    reg_write(p->base_addr, UART_LCRH, lcrh);
    reg_write(p->base_addr, UART_CR, CR_UARTEN | CR_TXE | CR_RXE);

    return DRV_OK;
}

static int pl011_init(device_t *dev)
{
    pl011_priv_t *p = dev->priv;
    if (!p || !p->base_addr || !p->config.baudrate)
    {
        return DRV_EINVAL;
    }
    return pl011_do_config(p);
}

static int pl011_deinit(device_t *dev)
{
    pl011_priv_t *p = dev->priv;
    while (reg_read(p->base_addr, UART_FR) & FR_BUSY)
        ;
    reg_write(p->base_addr, UART_CR, 0);
    return DRV_OK;
}

static int pl011_write(device_t *dev, const void *buf, size_t len)
{
    pl011_priv_t *p = dev->priv;
    const char *bytes = buf;
    for (size_t i = 0; i < len; i++)
    {
        while (reg_read(p->base_addr, UART_FR) & FR_TXFF)
            ;
        reg_write(p->base_addr, UART_DR, (uint32_t)bytes[i]);
    }
    return (int)len;
}

static int pl011_read(device_t *dev, void *buf, size_t len)
{
    pl011_priv_t *p = dev->priv;
    char *bytes = buf;
    size_t count = 0;
    while (count < len)
    {
        if (reg_read(p->base_addr, UART_FR) & FR_RXFE)
        {
            break;
        }
        bytes[count++] = (char)(reg_read(p->base_addr, UART_DR) & 0xFF);
    }
    return (int)count;
}

static int pl011_ioctl(device_t *dev, uint32_t cmd, void *arg)
{
    pl011_priv_t *p = dev->priv;

    switch (cmd)
    {

        case PL011_IOCTL_SET_BAUD:
        {
            p->config.baudrate = *(uint32_t *)arg;
            return pl011_do_config(p);
        }

        case PL011_IOCTL_SET_CONFIG:
        {
            p->config = *(pl011_config_t *)arg;
            return pl011_do_config(p);
        }

        case PL011_IOCTL_FLUSH:
        {
            while (reg_read(p->base_addr, UART_FR) & FR_BUSY)
                ;
            return DRV_OK;
        }

        case PL011_IOCTL_TX_FULL:
        {
            *(bool *)arg = (reg_read(p->base_addr, UART_FR) & FR_TXFF) != 0;
            return DRV_OK;
        }

        case PL011_IOCTL_RX_EMPTY:
        {
            *(bool *)arg = (reg_read(p->base_addr, UART_FR) & FR_RXFE) != 0;
            return DRV_OK;
        }

        case PL011_IOCTL_PRINT_HEX:
        {
            static const char hex[] = "0123456789ABCDEF";
            pl011_print_hex_arg_t *a = arg;
            int w = (a->width <= 0 || a->width > 64) ? 64 : a->width;
            const char prefix[] = "0x";
            pl011_write(dev, prefix, 2);
            for (int i = w - 4; i >= 0; i -= 4)
            {
                char nibble = hex[(a->value >> i) & 0xF];
                pl011_write(dev, &nibble, 1);
            }
            return DRV_OK;
        }
    }
    return DRV_ENOSYS;
}

const driver_ops_t pl011_ops = {
        .name = "pl011",
        .class = "uart",
        .init = pl011_init,
        .deinit = pl011_deinit,
        .read = pl011_read,
        .write = pl011_write,
        .ioctl = pl011_ioctl,
        .isr = NULL,
};

void pl011_register(const char *name, pl011_priv_t *priv)
{

}
