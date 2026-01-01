/**
 * @file pl011.h
 * @brief PL011 UART driver for ARM platforms
 *
 * Driver for ARM PrimeCell PL011 UART as found on QEMU virt machine
 * and other ARM-based platforms.
 */

#ifndef PL011_UART_H
#define PL011_UART_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Configuration - can be overridden in pl011_uart_config.h */
#ifndef UART0_BASE_ADDR
#define UART0_BASE_ADDR 0x09000000UL
#endif

#ifndef UART0_CLOCK_HZ
#define UART0_CLOCK_HZ 24000000UL /* 24 MHz default for QEMU virt */
#endif

#ifndef UART0_BAUDRATE
#define UART0_BAUDRATE 115200
#endif

/**
 * @brief UART data bits configuration
 */
typedef enum
{
    UART_DATA_5BITS = 0,
    UART_DATA_6BITS = 1,
    UART_DATA_7BITS = 2,
    UART_DATA_8BITS = 3
} uart_data_bits_t;

/**
 * @brief UART parity configuration
 */
typedef enum
{
    UART_PARITY_NONE = 0,
    UART_PARITY_ODD = 1,
    UART_PARITY_EVEN = 2
} uart_parity_t;

/**
 * @brief UART stop bits configuration
 */
typedef enum
{
    UART_STOP_1BIT = 0,
    UART_STOP_2BITS = 1
} uart_stop_bits_t;

/**
 * @brief UART configuration structure
 */
typedef struct
{
    uint32_t baudrate;
    uart_data_bits_t data_bits;
    uart_parity_t parity;
    uart_stop_bits_t stop_bits;
    bool enable_fifo;
} uart_config_t;

/**
 * @brief Initialize UART with default configuration
 *
 * Initializes UART with 115200 baud, 8N1, FIFO enabled
 *
 * @return 0 on success, negative error code on failure
 */
int pl011_uart_init(void);

/**
 * @brief Initialize UART with custom configuration
 *
 * @param config Pointer to configuration structure
 * @return 0 on success, negative error code on failure
 */
int pl011_uart_init_config(const uart_config_t *config);

/**
 * @brief Deinitialize UART
 */
void pl011_uart_deinit(void);

/**
 * @brief Write a single character to UART
 *
 * Blocks until character can be written
 *
 * @param c Character to write
 */
void pl011_uart_putc(char c);

/**
 * @brief Write a string to UART
 *
 * @param str Null-terminated string to write
 */
void pl011_uart_puts(const char *str);

/**
 * @brief Write buffer to UART
 *
 * @param data Pointer to data buffer
 * @param len Number of bytes to write
 * @return Number of bytes written
 */
size_t pl011_uart_write(const void *data, size_t len);

/**
 * @brief Read a single character from UART
 *
 * Blocks until character is available
 *
 * @return Character read
 */
char pl011_uart_getc(void);

/**
 * @brief Read a single character from UART (non-blocking)
 *
 * @param c Pointer to store character
 * @return true if character was read, false if no data available
 */
bool pl011_uart_getc_nonblocking(char *c);

/**
 * @brief Read buffer from UART
 *
 * @param buffer Pointer to buffer to store data
 * @param len Maximum number of bytes to read
 * @return Number of bytes read
 */
size_t pl011_uart_read(void *buffer, size_t len);

/**
 * @brief Check if transmit FIFO is full
 *
 * @return true if TX FIFO is full
 */
bool pl011_uart_tx_full(void);

/**
 * @brief Check if receive FIFO is empty
 *
 * @return true if RX FIFO is empty
 */
bool pl011_uart_rx_empty(void);

/**
 * @brief Flush transmit buffer
 *
 * Waits until all data has been transmitted
 */
void pl011_uart_flush(void);

/**
 * @brief Print hexadecimal value
 *
 * @param value Value to print
 * @param width Width in bits (8, 16, 32, or 64)
 */
void pl011_uart_print_hex(uint64_t value, int width);

#endif /* PL011_UART_H */
