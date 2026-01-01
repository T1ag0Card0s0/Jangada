# Jangada RTOS Documentation Index

Welcome to the Jangada RTOS documentation! This index will help you find what you're looking for.

## Quick Links

- **New to Jangada?** → Start with [Getting Started](GETTING_STARTED.md)
- **Want to understand the design?** → Read [Architecture](ARCHITECTURE.md)
- **Working with drivers?** → Check [Drivers](DRIVERS.md)
- **Building the project?** → See [Build System](BUILD_SYSTEM.md)
- **Contributing code?** → Review [Contributing](CONTRIBUTING.md)

## Documentation Structure

### 📖 [GETTING_STARTED.md](GETTING_STARTED.md)

**For**: New users and developers

**Contains**:
- Installation and setup instructions
- Building the kernel
- Running with QEMU
- Basic troubleshooting
- Development workflow

**Key Sections**:
- Prerequisites and compiler installation
- Build configuration
- Project structure overview
- Running and debugging the kernel
- Common issues and solutions

### 🏗️ [ARCHITECTURE.md](ARCHITECTURE.md)

**For**: Understanding system design

**Contains**:
- Layered architecture overview
- Core component descriptions
- Boot sequence
- Design patterns
- Memory layout
- Build system flow

**Key Sections**:
- Architecture layers (hardware → application)
- Component responsibilities
- Initialization sequence
- Memory organization
- Future enhancement plans

### 🔌 [DRIVERS.md](DRIVERS.md)

**For**: Hardware abstraction and driver development

**Contains**:
- TTY abstraction layer documentation
- PL011 UART driver details
- Complete API reference
- Usage examples
- Hardware specifications
- Adding new drivers guide

**Key Sections**:
- TTY subsystem architecture and API
- PL011 specifications and configuration
- Driver registration
- Debugging techniques
- Future driver plans

### 🛠️ [BUILD_SYSTEM.md](BUILD_SYSTEM.md)

**For**: Build configuration and customization

**Contains**:
- Build process explanation
- Makefile structure
- Configuration system
- Compiler flags
- Cross-compilation setup
- Customization guide

**Key Sections**:
- Top-level makefile targets
- Configuration hierarchy
- Subdirectory organization
- Library building
- Performance optimization
- Common build errors

### 👥 [CONTRIBUTING.md](CONTRIBUTING.md)

**For**: Contributing to the project

**Contains**:
- Development workflow
- Code style guidelines
- Testing procedures
- Pull request process
- Contribution areas
- Commit message format

**Key Sections**:
- Code of conduct
- Style conventions
- Testing requirements
- PR submission process
- Issues to work on
- Legal requirements

## Learning Path

### For Complete Beginners

1. **Start here**: [Getting Started](GETTING_STARTED.md)
   - Install cross-compiler
   - Build the project
   - Run in QEMU
   
2. **Then explore**: [Architecture](ARCHITECTURE.md)
   - Understand the design
   - Learn component relationships
   - See how it boots

3. **Dive deeper**: [Drivers](DRIVERS.md)
   - Understand TTY system
   - See how hardware is abstracted
   - Learn driver registration

### For Contributors

1. **Review**: [Contributing](CONTRIBUTING.md)
   - Understand contribution process
   - Learn code style
   - See testing requirements

2. **Familiarize**: [Architecture](ARCHITECTURE.md)
   - Understand how things fit together
   - See design patterns
   - Plan your changes

3. **Reference**: [Build System](BUILD_SYSTEM.md) or [Drivers](DRIVERS.md)
   - Depending on what you're working on

### For Integrating New Hardware

1. **Read**: [Architecture](ARCHITECTURE.md)
   - Understand boot process
   - Learn memory layout
   - See initialization flow

2. **Study**: [Drivers](DRIVERS.md)
   - See driver abstraction pattern
   - Learn PL011 as reference
   - Understand registration

3. **Reference**: [Build System](BUILD_SYSTEM.md)
   - Learn to add new directories
   - Configure cross-compiler
   - Optimize for your platform

## Common Tasks

### Building the Project

See: [Getting Started - Building the Project](GETTING_STARTED.md#building-the-project)

```bash
make                    # Standard build
make clean             # Remove artifacts
make ARCH=aarch64      # Specify architecture
```

### Running in QEMU

See: [Getting Started - Running the Kernel](GETTING_STARTED.md#running-the-kernel)

```bash
qemu-system-aarch64 -kernel build/jangada -m 1024 -nographic
```

### Debugging with GDB

See: [Getting Started - Development Workflow](GETTING_STARTED.md#development-workflow)

```bash
# Terminal 1
qemu-system-aarch64 -kernel build/jangada -m 1024 -nographic -S -gdb tcp::1234 &

# Terminal 2
aarch64-linux-gnu-gdb build/jangada -ex "target remote :1234"
```

### Writing a Driver

See: [Drivers - Adding New Drivers](DRIVERS.md#adding-new-drivers)

1. Define hardware abstraction
2. Implement hardware driver
3. Register with kernel

### Adding a Subsystem

See: [Build System - Adding New Subdirectory](BUILD_SYSTEM.md#adding-new-subdirectory)

1. Create directory structure
2. Create subdirectory Makefile
3. Update top-level Makefile
4. Implement functionality

### Submitting Code

See: [Contributing - Pull Request Process](CONTRIBUTING.md#pull-request-process)

1. Fork and clone
2. Create feature branch
3. Make changes with tests
4. Submit pull request
5. Respond to feedback

## Architecture Overview

```
Application Layer
        ↓
   Kernel (RTOS)
   - Scheduler
   - Tasks
   - Synchronization
        ↓
Device Drivers
   - TTY Abstraction
   - UART (PL011)
   - Future: GPIO, SPI, I2C
        ↓
Architecture-Specific
   - Startup Code (ARM64)
   - Linker Script
        ↓
     Hardware
   - CPU, Memory, Peripherals
```

See [Architecture](ARCHITECTURE.md) for detailed explanation.

## Supported Platforms

Currently:
- **ARM64** (aarch64): Full support with QEMU

Planned:
- ARM32 (armv7)
- RISC-V (rv64imac)
- Additional boards and platforms

See [Architecture - Future Enhancements](ARCHITECTURE.md#future-enhancements)

## Project Statistics

- **Lines of Code**: ~2,000 (core kernel + drivers)
- **Documentation**: ~500 lines across 5 files
- **Supported Architectures**: 1 (ARM64)
- **Drivers**: TTY, PL011 UART
- **License**: MIT

## Key Features (Current)

✅ Modular architecture
✅ Hardware abstraction layers
✅ ARM64 support
✅ QEMU compatibility
✅ TTY/UART interface
✅ Cross-compilation support

## Upcoming Features

🔲 Real-time task scheduler
🔲 Interrupt handling
🔲 Synchronization primitives
🔲 Dynamic memory management
🔲 Additional drivers
🔲 Additional architectures

## File Organization

```
Jangada/
├── README.md                    # Project overview
├── docs/
│   ├── index.md                 # This file
│   ├── GETTING_STARTED.md       # Setup & build guide
│   ├── ARCHITECTURE.md          # Design & structure
│   ├── DRIVERS.md               # Driver documentation
│   ├── BUILD_SYSTEM.md          # Build configuration
│   └── CONTRIBUTING.md          # Contribution guidelines
├── arch/                        # Architecture-specific code
│   └── aarch64/                 # ARM64 implementation
├── kernel/                      # RTOS kernel
├── drivers/                     # Hardware drivers
├── lib/                         # Standard library
├── init/                        # Kernel startup
├── include/                     # Public headers
├── config/                      # Build configuration
└── Makefile                     # Top-level build script
```

## Getting Help

- **Questions**: Check relevant documentation section
- **Issues**: Open on GitHub with clear description
- **Contact**: t.rafael03.tc@gmail.com
- **Pull Requests**: Follow [Contributing Guidelines](CONTRIBUTING.md)

## Contributing

We welcome contributions! See [Contributing Guidelines](CONTRIBUTING.md) for:
- Code style and standards
- Testing procedures
- Commit message format
- Pull request process
- Areas needing work

## License

Jangada RTOS is licensed under the MIT License.

---

**Last Updated**: January 2026
**Documentation Version**: 1.0
**Project Status**: Active Development
