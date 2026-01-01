# Getting Started with Jangada RTOS

## Prerequisites

Before building Jangada, ensure you have the following installed:

- **ARM Cross-Compiler**: `aarch64-linux-gnu-gcc` or compatible
- **Make**: GNU Make 3.81 or later
- **Git**: For cloning and version control

### Installing Cross-Compiler

#### Ubuntu/Debian
```bash
sudo apt-get install gcc-aarch64-linux-gnu
```

#### macOS (Homebrew)
```bash
brew install aarch64-unknown-linux-gnu
```

## Building the Project

### Basic Build

From the project root directory:

```bash
make
```

This will build the kernel and produce an executable at `build/jangada`.

### Clean Build

To remove all build artifacts:

```bash
make clean
```

Then rebuild:

```bash
make
```

### Build Configuration

The build system uses configuration files in the `config/` directory:

- **`config/default.config`**: Default architecture and toolchain settings
- **`config/make.global`**: Global make variables
- **`config/triplet.global`**: Cross-compiler triplet configuration

To use custom configuration:

```bash
make ARCH=aarch64 TRIPLET=aarch64-linux-gnu
```

## Project Structure

```
Jangada/
├── arch/              # Architecture-specific code
│   └── aarch64/       # ARM64 implementation
├── kernel/            # Core kernel functionality
├── drivers/           # Hardware drivers (TTY, UART, etc.)
├── lib/               # Standard library implementations
├── init/              # Kernel initialization
├── include/           # Public header files
├── config/            # Build configuration
├── docs/              # Documentation
└── scripts/           # Utility scripts
```

## Key Components

### Architecture Support

Currently supports:
- **ARM64 (aarch64)**: Full implementation with startup code and linker script

### Drivers

- **TTY Abstraction Layer**: Terminal/console interface
- **PL011 UART**: ARM PrimeCell UART driver

### Kernel Features

- Task management (framework in place)
- Real-time task scheduling (in development)

### Standard Library

Minimal libc implementation including:
- `string.h`: Memory and string operations
  - `memcpy()`: Memory copying
  - `memset()`: Memory initialization
  - `strcmp()`: String comparison

## Running the Kernel

To execute the kernel:

```bash
qemu-system-aarch64 -kernel build/jangada -m 1024 -nographic
```

**Note**: QEMU must be configured for ARM64 emulation.

## Development Workflow

1. **Make changes** to source files
2. **Rebuild** with `make`
3. **Test** using QEMU or your target hardware
4. **Debug** using GDB with QEMU:

```bash
qemu-system-aarch64 -kernel build/jangada -m 1024 -nographic -S -gdb tcp::1234 &
aarch64-linux-gnu-gdb build/jangada -ex "target remote :1234"
```

## Troubleshooting

### Build Errors

- **Missing cross-compiler**: Verify installation with `aarch64-linux-gnu-gcc --version`
- **Configuration errors**: Check `config/default.config` for correct `TRIPLET` and `ARCH`

### Execution Issues

- **QEMU not found**: Install with `sudo apt-get install qemu-system-arm`
- **Permission denied**: Ensure build artifacts have correct permissions

## Next Steps

- Read [Architecture Overview](architecture.md) for design details
- Check [Driver Documentation](drivers.md) for hardware interface info
- Review source code in respective directories for implementation details

## Support

For issues and questions, contact: t.rafael03.tc@gmail.com
