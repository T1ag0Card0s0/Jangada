# Jangada RTOS Drivers

## Overview

The driver subsystem provides hardware abstraction layers that allow applications to interact with peripheral devices without needing to understand underlying hardware details.

## TTY (Terminal) Subsystem

### Purpose

The TTY subsystem provides a unified interface for character-based input/output devices (terminals, serial ports, etc.). This abstraction allows multiple different hardware implementations to be used interchangeably.

### Architecture

```
Application Code
       │
       ▼
   TTY API (tty.h)
       │
       ├─ tty_init()
       ├─ tty_open()
       ├─ tty_close()
       ├─ tty_puts()
       ├─ tty_gets()
       └─ tty_putc()
       │
       ▼
   TTY Device Layer (tty_device_t)
       │
       ├─ Device Operations
       │  (open, close, putc, getc)
       │
       ├─ Private Driver Data
       │
       ▼
   Hardware Driver (PL011 UART)
       │
       ▼
   UART Hardware (Registers)
```

### TTY API Reference

#### Initialization

```c
void tty_init(void);
```

Initialize the TTY subsystem. Must be called before any other TTY functions.

**Parameters**: None
**Returns**: None

#### Device Management

```c
tty_device_t *tty_open(tty_device_t *dev);
```

Open a TTY device and initialize underlying hardware.

**Parameters**:
- `dev`: Pointer to TTY device structure

**Returns**: Pointer to opened device, or NULL on error

```c
int tty_close(tty_device_t *dev);
```

Close a TTY device and release hardware resources.

**Parameters**:
- `dev`: Pointer to open TTY device

**Returns**: 0 on success, negative on error

#### Output Operations

```c
int tty_putc(tty_device_t *dev, char c);
```

Write a single character to the device.

**Parameters**:
- `dev`: Pointer to TTY device
- `c`: Character to write

**Returns**: Character written, or -1 on error

```c
int tty_puts(tty_device_t *dev, const char *str);
```

Write a null-terminated string to the device.

**Parameters**:
- `dev`: Pointer to TTY device
- `str`: String to write (must be null-terminated)

**Returns**: Number of characters written, or -1 on error

#### Input Operations

```c
int tty_getc(tty_device_t *dev);
```

Read a single character from the device (blocking).

**Parameters**:
- `dev`: Pointer to TTY device

**Returns**: Character read (0-255), or -1 on error

```c
int tty_gets(tty_device_t *dev, char *buf, size_t size);
```

Read a line from the device in canonical mode (line-buffered).

Waits for a complete line (terminated by newline or carriage return).

**Parameters**:
- `dev`: Pointer to TTY device
- `buf`: Buffer for input data
- `size`: Size of buffer

**Returns**: Number of characters read, or -1 on error

### Example Usage

```c
#include "drivers/tty.h"

extern tty_device_t *tty_pl011_get(void);

int main(void) {
    // Initialize TTY subsystem
    tty_init();
    
    // Get console device
    tty_device_t *console = tty_pl011_get();
    
    // Open device
    if (!tty_open(console)) {
        return -1;
    }
    
    // Write to console
    tty_puts(console, "Hello, World!\n");
    
    // Read from console
    char buffer[64];
    if (tty_gets(console, buffer, sizeof(buffer)) > 0) {
        tty_puts(console, "You said: ");
        tty_puts(console, buffer);
    }
    
    // Close device
    tty_close(console);
    
    return 0;
}
```

## PL011 UART Driver

### Overview

The PL011 (ARM PrimeCell UART) driver provides low-level access to UART hardware compatible with the ARM PrimeCell specification.

### Hardware Specifications

**Device**: ARM PrimeCell UART (PL011)

**Key Features**:
- 32-bit data interface
- Programmable baud rates (up to 921.6 kbps)
- FIFOs for transmit and receive
- Programmable data width, stop bits, parity
- Hardware flow control (RTS/CTS)
- Interrupt support

### Register Map

```c
struct pl011_regs {
    uint32_t data;          // 0x00: Data Register
    uint32_t rsr_ecr;       // 0x04: Receive Status / Error Clear
    uint32_t reserved1[4];  // 0x08-0x14: Reserved
    uint32_t fr;            // 0x18: Flag Register
    uint32_t reserved2;     // 0x1C: Reserved
    uint32_t ilpr;          // 0x20: UART IrDA Low-power Counter
    uint32_t ibrd;          // 0x24: Integer Baud Rate Divisor
    uint32_t fbrd;          // 0x28: Fractional Baud Rate Divisor
    uint32_t lcrh;          // 0x2C: Line Control Register High
    uint32_t cr;            // 0x30: Control Register
    uint32_t ifls;          // 0x34: Interrupt FIFO Level Select
    uint32_t imsc;          // 0x38: Interrupt Mask Set/Clear
    uint32_t ris;           // 0x3C: Raw Interrupt Status
    uint32_t mis;           // 0x40: Masked Interrupt Status
    uint32_t icr;           // 0x44: Interrupt Clear Register
    uint32_t dmacr;         // 0x48: DMA Control Register
};
```

### Configuration

**Default Settings** (from PL011 initialization):
- Baud Rate: 115200
- Data Bits: 8
- Stop Bits: 1
- Parity: None
- Flow Control: Disabled

### Hardware Base Addresses

Standard ARM64 QEMU configuration:
- **Console UART**: `0x09000000`

To modify base address, update in driver source or pass during initialization.

### Low-Level API

```c
// Initialize PL011 hardware
int pl011_init(unsigned long base_addr, unsigned int baudrate);

// Read/write single character
unsigned char pl011_readc(void);
void pl011_writec(unsigned char c);

// Query hardware status
int pl011_tx_ready(void);
int pl011_rx_ready(void);
```

### Device Registration

The PL011 driver is automatically registered during kernel initialization:

```c
// In init/main.c
tty_pl011_init("console", 256);  // Register as "console" with 256-byte buffer
tty_device_t *console = tty_pl011_get();
```

## Adding New Drivers

To add a new driver to Jangada:

### 1. Define Hardware Abstraction

Create a header file defining the device operations:

```c
// include/drivers/my_device.h
typedef struct {
    int (*init)(void);
    int (*read)(char *buf, size_t size);
    int (*write)(const char *buf, size_t size);
    void (*close)(void);
} my_device_ops_t;
```

### 2. Implement Hardware Driver

```c
// drivers/my_device/my_device.c
static int my_device_init(void) {
    // Hardware initialization
    return 0;
}

static int my_device_write(const char *buf, size_t size) {
    // Hardware write
    return size;
}

// ... other operations
```

### 3. Register with Kernel

In `init/main.c`, call device initialization:

```c
void start_kernel(void) {
    my_device_init();
    // ... other initialization
}
```

## Debugging Drivers

### Using GDB with QEMU

```bash
# Terminal 1: Start QEMU with GDB stub
qemu-system-aarch64 -kernel build/jangada -m 1024 -nographic -S -gdb tcp::1234 &

# Terminal 2: Connect GDB
aarch64-linux-gnu-gdb build/jangada
(gdb) target remote :1234
(gdb) break pl011_writec
(gdb) continue
```

### Common Issues

| Issue | Cause | Solution |
|-------|-------|----------|
| No output from UART | Hardware not initialized | Check pl011_init() called with correct base address |
| Garbled output | Wrong baud rate | Verify baud rate matches host terminal |
| Freeze on input | Buffer full | Increase buffer size or check line discipline |
| Driver not found | Not registered | Verify tty_pl011_init() called in start_kernel() |

## Future Driver Plans

- **GPIO**: General Purpose Input/Output
- **SPI**: Serial Peripheral Interface for sensors/displays
- **I2C**: Inter-IC Communication bus
- **Timer**: System timer with interrupt support
- **Interrupt Controller**: GIC (ARM Generic Interrupt Controller)

## References

- ARM PrimeCell UART (PL011) DDI 0183F
- QEMU ARM64 Emulation Documentation
- ARM Architecture Reference Manual (ARMv8)
