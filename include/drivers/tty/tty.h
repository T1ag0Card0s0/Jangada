/**
 * @file tty.h
 * @brief TTY (Terminal) abstraction layer
 *
 * Provides a hardware-independent TTY interface that can work with
 * multiple underlying UART drivers or other character devices.
 */

#ifndef TTY_H
#define TTY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Forward declaration */
struct tty_device;

/**
 * @brief TTY device operations structure
 *
 * Function pointers to hardware-specific operations
 */
typedef struct
{
    int (*init)(void);
    void (*deinit)(void);
    void (*putc)(char c);
    char (*getc)(void);
    bool (*getc_nonblocking)(char *c);
    size_t (*write)(const void *data, size_t len);
    size_t (*read)(void *buffer, size_t len);
    bool (*tx_ready)(void);
    bool (*rx_available)(void);
    void (*flush)(void);
} tty_ops_t;

/**
 * @brief TTY flags
 */
typedef enum
{
    TTY_FLAG_ECHO = (1u << 0),      /* Echo input characters */
    TTY_FLAG_CANONICAL = (1u << 1), /* Line buffering mode */
    TTY_FLAG_CRLF = (1u << 2),      /* Convert LF to CRLF on output */
    TTY_FLAG_CRLFNL = (1u << 3),    /* Convert CR to LF on input */
} tty_flags_t;

/**
 * @brief TTY device structure
 */
typedef struct tty_device
{
    const char *name;     /* Device name (e.g., "tty0", "console") */
    const tty_ops_t *ops; /* Hardware operations */
    uint32_t flags;       /* Configuration flags */

    /* Line buffer for canonical mode */
    char *line_buffer;
    size_t line_buffer_size;
    size_t line_buffer_pos;

    /* Statistics */
    uint64_t bytes_written;
    uint64_t bytes_read;

    /* Optional mutex for thread safety */
    void *mutex; /* RTOS-specific mutex handle */

    /* User data */
    void *user_data;
} tty_device_t;

/**
 * @brief Initialize TTY subsystem
 *
 * @return 0 on success, negative error code on failure
 */
int tty_init(void);

/**
 * @brief Register a TTY device
 *
 * @param tty Pointer to TTY device structure
 * @param name Device name
 * @param ops Hardware operations
 * @param line_buffer_size Size of line buffer (0 to disable canonical mode)
 * @return 0 on success, negative error code on failure
 */
int tty_register(tty_device_t *tty, const char *name, const tty_ops_t *ops,
                 size_t line_buffer_size);

/**
 * @brief Unregister a TTY device
 *
 * @param tty Pointer to TTY device structure
 */
void tty_unregister(tty_device_t *tty);

/**
 * @brief Get TTY device by name
 *
 * @param name Device name
 * @return Pointer to TTY device or NULL if not found
 */
tty_device_t *tty_get(const char *name);

/**
 * @brief Open a TTY device
 *
 * @param tty Pointer to TTY device
 * @return 0 on success, negative error code on failure
 */
int tty_open(tty_device_t *tty);

/**
 * @brief Close a TTY device
 *
 * @param tty Pointer to TTY device
 */
void tty_close(const tty_device_t *tty);

/**
 * @brief Write a character to TTY
 *
 * @param tty Pointer to TTY device
 * @param c Character to write
 */
void tty_putc(tty_device_t *tty, char c);

/**
 * @brief Write a string to TTY
 *
 * @param tty Pointer to TTY device
 * @param str Null-terminated string
 */
void tty_puts(tty_device_t *tty, const char *str);

/**
 * @brief Write data to TTY
 *
 * @param tty Pointer to TTY device
 * @param data Pointer to data
 * @param len Number of bytes to write
 * @return Number of bytes written
 */
size_t tty_write(tty_device_t *tty, const void *data, size_t len);

/**
 * @brief Read a character from TTY
 *
 * Blocking read
 *
 * @param tty Pointer to TTY device
 * @return Character read
 */
char tty_getc(tty_device_t *tty);

/**
 * @brief Read a character from TTY (non-blocking)
 *
 * @param tty Pointer to TTY device
 * @param c Pointer to store character
 * @return true if character was read, false otherwise
 */
bool tty_getc_nonblocking(tty_device_t *tty, char *c);

/**
 * @brief Read a line from TTY
 *
 * Reads until newline or buffer is full. Only works in canonical mode.
 *
 * @param tty Pointer to TTY device
 * @param buffer Buffer to store line
 * @param size Size of buffer
 * @return Number of bytes read (excluding null terminator)
 */
size_t tty_readline(tty_device_t *tty, char *buffer, size_t size);

/**
 * @brief Read data from TTY
 *
 * @param tty Pointer to TTY device
 * @param buffer Buffer to store data
 * @param len Maximum number of bytes to read
 * @return Number of bytes read
 */
size_t tty_read(tty_device_t *tty, void *buffer, size_t len);

/**
 * @brief Flush TTY output
 *
 * @param tty Pointer to TTY device
 */
void tty_flush(tty_device_t *tty);

/**
 * @brief Set TTY flags
 *
 * @param tty Pointer to TTY device
 * @param flags Flags to set (OR'd tty_flags_t values)
 */
void tty_set_flags(tty_device_t *tty, uint32_t flags);

/**
 * @brief Clear TTY flags
 *
 * @param tty Pointer to TTY device
 * @param flags Flags to clear (OR'd tty_flags_t values)
 */
void tty_clear_flags(tty_device_t *tty, uint32_t flags);

/**
 * @brief Get TTY flags
 *
 * @param tty Pointer to TTY device
 * @return Current flags
 */
uint32_t tty_get_flags(tty_device_t *tty);

/**
 * @brief Printf-style formatted output to TTY
 *
 * @param tty Pointer to TTY device
 * @param fmt Format string
 * @param ... Variable arguments
 * @return Number of characters written
 */
int tty_printf(tty_device_t *tty, const char *fmt, ...);

/**
 * @brief Print hexadecimal value to TTY
 *
 * @param tty Pointer to TTY device
 * @param value Value to print
 * @param width Width in bits (8, 16, 32, or 64)
 */
void tty_print_hex(tty_device_t *tty, uint64_t value, int width);

/**
 * @brief Check if data is available to read
 *
 * @param tty Pointer to TTY device
 * @return true if data is available
 */
bool tty_rx_available(tty_device_t *tty);

/**
 * @brief Check if TTY is ready to transmit
 *
 * @param tty Pointer to TTY device
 * @return true if ready to transmit
 */
bool tty_tx_ready(tty_device_t *tty);

/* Global default console (can be set by application) */
extern tty_device_t *tty_console;

/* Convenience macros for default console */
#define console_putc(c) tty_putc(tty_console, c)
#define console_puts(s) tty_puts(tty_console, s)
#define console_write(d, l) tty_write(tty_console, d, l)
#define console_getc() tty_getc(tty_console)
#define console_read(b, l) tty_read(tty_console, b, l)
#define console_printf(...) tty_printf(tty_console, __VA_ARGS__)
#define console_print_hex(v, w) tty_print_hex(tty_console, v, w)

#endif /* TTY_H */
