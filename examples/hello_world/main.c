// Minimal AArch64 QEMU 'virt' UART test — single file version
#include <stdint.h>

/* --- UART definitions (PL011 at 0x0900_0000) --- */
#define UART0_BASE   0x09000000UL

#define UART_DR      0x00
#define UART_FR      0x18
#define UART_IBRD    0x24
#define UART_FBRD    0x28
#define UART_LCRH    0x2C
#define UART_CR      0x30
#define UART_IMSC    0x38
#define UART_ICR     0x44

#define FR_TXFF      (1u << 5)
#define FR_BUSY      (1u << 3)
#define LCRH_FEN     (1u << 4)
#define LCRH_WLEN_8  (3u << 5)
#define CR_UARTEN    (1u << 0)
#define CR_TXE       (1u << 8)
#define CR_RXE       (1u << 9)

static inline void mmio_write(uint64_t reg, uint32_t val) {
    *(volatile uint32_t *)(reg) = val;
}
static inline uint32_t mmio_read(uint64_t reg) {
    return *(volatile uint32_t *)(reg);
}
static inline uint64_t UART(uint32_t off) { return (UART0_BASE + off); }

/* --- UART driver --- */
static void uart_init(void) {
    mmio_write(UART(UART_CR), 0);
    mmio_write(UART(UART_ICR), 0x7FF);
    mmio_write(UART(UART_IBRD), 13);
    mmio_write(UART(UART_FBRD), 2);
    mmio_write(UART(UART_LCRH), LCRH_WLEN_8 | LCRH_FEN);
    mmio_write(UART(UART_CR), CR_UARTEN | CR_TXE | CR_RXE);
}

static void uart_putc(char c) {
    if (c == '\n')
        uart_putc('\r');
    while (mmio_read(UART(UART_FR)) & FR_TXFF) {}
    mmio_write(UART(UART_DR), (uint32_t)c);
}

static void uart_puts(const char *s) {
    while (*s)
        uart_putc(*s++);
}

static void uart_print_hex64(uint64_t x) {
    static const char hex[] = "0123456789ABCDEF";
    uart_puts("0x");
    for (int i = 60; i >= 0; i -= 4)
        uart_putc(hex[(x >> i) & 0xF]);
}

/* --- Main --- */
extern char _stack_top_; // from linker script

int main(void) {
    uart_init();
    uart_puts("Hello from Jangada RTOS on AArch64!\n");
    uart_puts("Stack top @ ");
    uart_print_hex64((uint64_t)&_stack_top_);
    uart_puts("\n");

    while (1) { }
}

