# Contributing to Jangada RTOS

Thank you for your interest in contributing to Jangada RTOS! This document provides guidelines for contributing to the project.

## Code of Conduct

Be respectful and constructive in all interactions. We're building a community around a shared passion for real-time operating systems.

## Getting Started

### 1. Fork and Clone

```bash
git clone https://github.com/YOUR_USERNAME/Jangada.git
cd Jangada
```

### 2. Set Up Development Environment

```bash
# Install cross-compiler
sudo apt-get install gcc-aarch64-linux-gnu

# Verify build
make clean
make
```

### 3. Create a Branch

```bash
git checkout -b feature/your-feature-name
# or
git checkout -b fix/issue-description
```

## Development Workflow

### Before Starting

1. **Check existing issues** to avoid duplicate work
2. **Discuss major changes** by opening an issue first
3. **Read relevant documentation** in `docs/`

### While Coding

#### Code Style

Follow these guidelines:

- **Indentation**: Use 4 spaces (no tabs)
- **Line Length**: Keep under 100 characters when possible
- **Comments**: Document complex logic
  ```c
  // Single-line comment for simple explanations
  
  /* Multi-line comment
     for more detailed explanations
     of complex algorithms */
  ```
- **Function Documentation**: Use doc-style comments
  ```c
  /**
   * @brief Short description
   * @param param1 Description
   * @return Description of return value
   */
  int my_function(int param1);
  ```

#### Naming Conventions

- **Functions**: `snake_case` (e.g., `tty_open()`, `pl011_init()`)
- **Variables**: `snake_case` (e.g., `buffer_size`, `uart_base`)
- **Constants**: `UPPER_SNAKE_CASE` (e.g., `UART_BASE_ADDR`)
- **Types**: `snake_case_t` (e.g., `tty_device_t`)
- **Macros**: `UPPER_SNAKE_CASE` (e.g., `MAX_BUFFER_SIZE`)

#### File Organization

- **Headers**: Place in `include/` with directory structure mirroring `src/`
- **Sources**: Place in appropriate component directory (`drivers/`, `kernel/`, etc.)
- **Private headers**: Keep in source directory with component code

Example structure for new driver:
```
drivers/my_device/
├── Makefile
├── my_device.c
└── my_device.h

include/drivers/
└── my_device.h
```

### Testing Your Changes

#### Build Tests

```bash
# Clean build
make clean
make

# Verify no warnings
make 2>&1 | grep -i warning
```

#### Runtime Testing

```bash
# Build
make

# Test with QEMU
qemu-system-aarch64 -kernel build/jangada -m 1024 -nographic

# Expected output
# "Hello from TTY abstraction!"
```

#### GDB Debugging

```bash
# Terminal 1: Start QEMU with GDB
qemu-system-aarch64 -kernel build/jangada -m 1024 -nographic -S -gdb tcp::1234 &

# Terminal 2: Debug
aarch64-linux-gnu-gdb build/jangada -ex "target remote :1234"
```

### Commit Messages

Write clear, descriptive commit messages:

```
feat: add GPIO driver for ARM64

- Implement GPIO register access
- Support input/output modes
- Add interrupt handlers

Closes #123
```

**Format**:
- First line: type and brief description (50 chars max)
- Types: `feat`, `fix`, `docs`, `refactor`, `test`, `chore`
- Body: Explain what and why (wrap at 72 chars)
- Footer: Reference issues with `Closes #123`

### Pull Request Process

1. **Push to your fork**:
   ```bash
   git push origin feature/your-feature-name
   ```

2. **Create pull request** on GitHub with:
   - Clear title and description
   - Reference to related issues
   - Explanation of changes
   - Testing performed

3. **Respond to review** feedback promptly

4. **Keep branch updated**:
   ```bash
   git fetch upstream
   git rebase upstream/main
   git push origin feature/your-feature-name --force
   ```

## Areas for Contribution

### High Priority

- [ ] Real-time task scheduler implementation
- [ ] Interrupt handling system
- [ ] Synchronization primitives (mutexes, semaphores)
- [ ] Dynamic memory management (malloc/free)
- [ ] Additional UART drivers
- [ ] Documentation improvements

### Medium Priority

- [ ] GPIO driver
- [ ] Timer driver
- [ ] SPI/I2C drivers
- [ ] Interrupt controller (GIC)
- [ ] Additional architectures (ARM32, RISC-V)
- [ ] Build system improvements

### Good First Issues

- Documentation corrections
- Code style improvements
- Test coverage
- Examples and tutorials
- Makefile optimizations

## Documentation Standards

### Code Documentation

All public APIs must be documented:

```c
/**
 * @file tty.h
 * @brief Terminal abstraction layer
 * 
 * Provides hardware-independent TTY interface
 * for character-based I/O.
 */

/**
 * @brief Initialize TTY subsystem
 * 
 * Sets up the TTY device list and initializes
 * the abstraction layer. Must be called before
 * any other TTY functions.
 * 
 * @return 0 on success, -1 on error
 */
int tty_init(void);
```

### Markdown Documentation

Keep documentation updated:

- Update `docs/` files when adding features
- Include examples in documentation
- Keep references current
- Use clear, concise language

## Issue Reporting

When reporting bugs:

1. **Search existing issues** first
2. **Provide details**:
   - Describe the problem clearly
   - Include steps to reproduce
   - Show expected vs actual behavior
   - Provide relevant code snippets

Example:
```markdown
## Bug: TTY output freezes on input

### Steps to Reproduce
1. Build project with `make`
2. Run in QEMU: `qemu-system-aarch64 -kernel build/jangada`
3. Wait for "Hello from TTY" message
4. Type input and press Enter

### Expected
System should echo input

### Actual
System freezes after first character input

### Environment
- OS: Linux
- Compiler: aarch64-linux-gnu-gcc 11.2.0
- QEMU: 7.0.0
```

## Feature Requests

For feature suggestions:

1. **Describe use case**: Why is this needed?
2. **Suggest implementation**: How might it work?
3. **Consider compatibility**: Does it break existing code?
4. **Provide examples**: Show how it would be used

## Performance Contributions

When optimizing code:

1. **Measure before/after**: Use profiling tools
2. **Document tradeoffs**: Speed vs size, complexity
3. **Test thoroughly**: Ensure no regressions
4. **Provide benchmarks**: Show improvement metrics

## Security

- Report security vulnerabilities privately to maintainers
- Don't disclose vulnerabilities in public issues
- Allow time for fixes before public disclosure

## Legal

- Ensure contributed code is your own or properly licensed
- Contributions are under the same license as the project (MIT)
- Include copyright header if adding significant files:

```c
/*
 * Copyright (c) 2026 Your Name
 * 
 * SPDX-License-Identifier: MIT
 */
```

## Resources

- [Architecture Documentation](ARCHITECTURE.md)
- [Getting Started Guide](GETTING_STARTED.md)
- [Driver Development](DRIVERS.md)
- [Build System](BUILD_SYSTEM.md)

## Questions?

- Open a discussion on GitHub
- Contact: t.rafael03.tc@gmail.com
- Check existing issues and documentation

Thank you for contributing to Jangada RTOS! 🛶
