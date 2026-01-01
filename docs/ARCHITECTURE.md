# Jangada RTOS Architecture

## Overview

Jangada is a modular, lightweight RTOS designed for embedded systems with minimal overhead. The architecture emphasizes:

- **Modularity**: Clear separation of concerns between layers
- **Portability**: Architecture-specific code isolated in dedicated directories
- **Efficiency**: Minimal resource overhead with maximum control

## Layered Architecture

```
┌─────────────────────────────────────────┐
│         Application Layer               │
│    (User programs and tasks)            │
└─────────────────────────────────────────┘
                  ▲
                  │
┌─────────────────────────────────────────┐
│      Kernel Layer (RTOS)                │
│  - Task Scheduler                       │
│  - Synchronization Primitives           │
│  - Memory Management                    │
└─────────────────────────────────────────┘
                  ▲
                  │
┌─────────────────────────────────────────┐
│      Device Driver Layer                │
│  - TTY/Console Abstraction              │
│  - UART Drivers (PL011)                 │
│  - Other Hardware Drivers               │
└─────────────────────────────────────────┘
                  ▲
                  │
┌─────────────────────────────────────────┐
│    Architecture-Specific Layer          │
│  - Startup Code (asm)                   │
│  - Memory Layout (linker script)        │
│  - CPU-specific features                │
└─────────────────────────────────────────┘
                  ▲
                  │
┌─────────────────────────────────────────┐
│          Hardware (CPU, Memory)         │
└─────────────────────────────────────────┘
```

## Core Components

### 1. Initialization (`init/`)

**Entry Point**: `init/main.c` → `start_kernel()`

Responsibilities:
- Initialize TTY/console subsystem
- Register hardware drivers
- Start kernel scheduler
- Enter main event loop

### 2. Kernel (`kernel/`)

**Core Runtime System**

#### Task Management (`kernel/task.c`)
- Framework for task creation and destruction
- Task state management (ready, running, blocked, etc.)
- Context switching infrastructure

#### Scheduler (planned)
- Priority-based task scheduling
- Time-slice management for cooperative multitasking

### 3. Drivers (`drivers/`)

#### TTY Abstraction Layer (`drivers/tty.c`)

A hardware-independent terminal interface providing:
- `tty_init()`: Initialize TTY subsystem
- `tty_open(device)`: Open a TTY device
- `tty_close(device)`: Close a TTY device
- `tty_puts(device, string)`: Write string to device
- `tty_gets(device, buffer, size)`: Read line from device
- Canonical mode support (line buffering)

**Key Design**: Device operations through function pointers allow multiple UART drivers to be plugged in.

#### PL011 UART Driver (`drivers/tty/uart/pl011.c`)

Implements the ARM PrimeCell UART interface:
- Hardware register access and control
- Interrupt handling (framework)
- Baud rate configuration
- Character transmission/reception

#### TTY Device Implementation (`drivers/tty/tty_pl011.c`)

Glue code connecting PL011 hardware driver to TTY abstraction:
- Line buffering for canonical mode
- Character-by-character buffering
- Integration with TTY operations

### 4. Architecture-Specific Code (`arch/`)

#### ARM64 (aarch64/)

**Startup Code** (`arch/aarch64/startup.S`):
- CPU initialization
- Stack setup
- Entry into `start_kernel()`

**Linker Script** (`include/arch/aarch64/linker.ld`):
- Memory section definitions
- Symbol placement
- Stack/heap configuration

### 5. Standard Library (`lib/`)

Minimal C library implementation:

#### String Operations (`lib/libc/string/`)
- `memcpy()`: Copy memory blocks (optimized)
- `memset()`: Fill memory with value
- `strcmp()`: Compare strings

## Build System

The build system uses **recursive makefiles** with the following structure:

```
Main Makefile (project root)
  ├── config/        (Build configuration, shared variables)
  ├── arch/Makefile  (Architecture-specific builds)
  ├── kernel/Makefile
  ├── drivers/Makefile
  ├── lib/Makefile
  └── init/Makefile
```

### Build Flow

1. **Configuration Loading**: `config/default.config` sets architecture and compiler
2. **Subdirectory Builds**: Each subdirectory compiles its objects
3. **Library Linking**: Libraries are built first (from `lib/`)
4. **Final Link**: All objects and libraries linked with linker script
5. **Output**: Executable at `build/jangada`

### Key Build Variables

- `ARCH`: Target architecture (e.g., `aarch64`)
- `TRIPLET`: Cross-compiler triplet (e.g., `aarch64-linux-gnu`)
- `BUILD_DIR`: Output directory for build artifacts
- `LDFLAGS`: Linker flags including linker script

## Memory Layout

Defined in `include/arch/aarch64/linker.ld`:

```
0x0000_0000  ├─────────────────┐
             │  Kernel Code    │
             │  & Data         │
0x4000_0000  ├─────────────────┐
             │  Heap           │
0x8000_0000  ├─────────────────┐
             │  Stack          │
0xFFFF_FFFF  └─────────────────┘
```

## Boot Sequence

```
1. Hardware reset → aarch64 startup code
2. Initialize CPU, stack
3. Call start_kernel()
4. Initialize TTY subsystem
5. Register drivers (PL011 UART)
6. Open console device
7. Enter main loop (idle)
```

## Design Patterns

### 1. Hardware Abstraction

Device drivers implement abstract operations through function pointers:

```c
struct tty_device {
    // Abstract operations
    int (*open)(struct tty_device *dev);
    int (*close)(struct tty_device *dev);
    int (*putc)(struct tty_device *dev, char c);
    int (*getc)(struct tty_device *dev);
    
    // Driver-specific data
    void *private;
};
```

### 2. Modular Initialization

Each subsystem handles its own initialization:
- TTY layer initializes abstraction
- PL011 driver initializes hardware
- TTY device layer registers with TTY subsystem

### 3. Separation of Concerns

- **Hardware** (PL011): Low-level register manipulation
- **Driver** (tty_pl011): Hardware registration
- **Abstraction** (tty): High-level interface
- **User Code**: Uses TTY interface only

## Future Enhancements

- **Interrupt Handling**: Currently in framework stage
- **Real-time Scheduling**: Priority-based scheduling
- **Synchronization**: Mutexes, semaphores, condition variables
- **Dynamic Memory Management**: malloc/free implementation
- **Additional Architectures**: ARM32, RISC-V support
- **Additional Drivers**: SPI, I2C, GPIO

## References

- **ARM PrimeCell UART (PL011)**: ARM DDI 0183F specification
- **ARM64 ABI**: ARM64 Application Binary Interface
- **GCC Cross-Compilation**: GNU toolchain documentation
