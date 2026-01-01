# Jangada Build System

## Overview

Jangada uses a **recursive make** build system designed for modularity and extensibility. Each subdirectory manages its own compilation while the top-level makefile orchestrates the overall build process.

## Top-Level Makefile

**Location**: `/Makefile`

### Key Targets

| Target | Description |
|--------|-------------|
| `make` or `make all` | Build the complete kernel executable |
| `make clean` | Remove all build artifacts |
| `make show-info` | Display build configuration |
| `make install` | Install kernel (if implemented) |

### Build Variables

```makefile
BUILD_DIR ?= build          # Output directory for build artifacts
TARGET := $(BUILD_DIR)/jangada  # Final executable name
TRIPLET := aarch64-linux-gnu    # Cross-compiler triplet
ARCH := aarch64             # Target architecture
```

### Build Configuration Files

Loaded from `config/` directory in order:

1. **`config/default.config`**: Sets `TRIPLET` and `ARCH`
2. **`config/make.global`**: Global make variables and functions
3. **`config/helpers.global`**: Helper functions (e.g., `check-required-vars`)
4. **`config/submake.global`**: Flags for submakefiles

## Configuration System

### config/default.config

Sets the target platform:

```makefile
TRIPLET := aarch64-linux-gnu
ARCH := aarch64
```

To build for different targets, modify these variables or pass on command line:

```bash
make ARCH=aarch64 TRIPLET=aarch64-linux-gnu
```

### config/make.global

Defines global build variables and rules used by all submakefiles:

```makefile
# Compiler variables
CC := $(TRIPLET)-gcc
AR := $(TRIPLET)-ar
AS := $(TRIPLET)-as
LD := $(TRIPLET)-ld

# Compile flags
CFLAGS := -march=armv8-a -Wall -Wextra -Werror

# Include directories
INCLUDES := -I$(PROJECT_PATH)/include
```

### config/helpers.global

Helper functions for validation and error checking:

```makefile
# Verify required variables are defined
define check-required-vars
  # Implementation
endef
```

### config/submake.global

Architecture-specific flags and settings loaded by subdirectory makefiles:

```makefile
# ARM64-specific settings
CFLAGS_ARCH := -march=armv8-a -mtune=cortex-a72
ASFLAGS_ARCH := -march=armv8-a
```

## Subdirectory Makefiles

Each major component has its own `Makefile`:

```
Makefile
├── arch/aarch64/Makefile
├── kernel/Makefile
├── drivers/Makefile
├── lib/Makefile
└── init/Makefile
```

### Subdirectory Structure

Each subdirectory makefile follows this pattern:

```makefile
# 1. Include top-level configuration
include $(PROJECT_PATH)/config/make.global
include $(PROJECT_PATH)/config/submake.global

# 2. Define local variables
LOCAL_DIR := arch/aarch64
LOCAL_OBJS := startup.o

# 3. Define build rules
$(LOCAL_DIR)/startup.o: $(LOCAL_DIR)/startup.S
    $(AS) $(ASFLAGS) -o $@ $<

# 4. Define targets
all: $(LOCAL_OBJS)

clean:
    rm -f $(LOCAL_OBJS)
```

## Build Process

### 1. Configuration Phase

```bash
make show-info
```

Validates configuration:
- ✓ TRIPLET is defined
- ✓ ARCH is defined
- ✓ Cross-compiler is available

### 2. Compilation Phase

```bash
make
```

Executes in order:

```
1. build-subdirs
   ├─ arch/aarch64/
   ├─ kernel/
   ├─ drivers/
   ├─ lib/           (LIBRARY - built with ar)
   └─ init/

2. find-all-objs    (Collect .o files)
3. find-all-libs    (Collect .a files)
4. Link Phase       (Combine with ld)
```

### 3. Linking Phase

Links all objects and libraries with the linker script:

```bash
aarch64-linux-gnu-gcc \
    -T include/arch/aarch64/linker.ld \
    -nostdlib -nostartfiles -ffreestanding \
    -Wl,--gc-sections \
    obj/arch/aarch64/startup.o \
    obj/kernel/task.o \
    obj/init/main.o \
    obj/drivers/tty.o \
    lib/libc.a \
    -o build/jangada
```

## Object File Organization

Object files are collected into `obj/` directory mirroring source structure:

```
obj/
├── arch/
│   └── aarch64/
│       └── startup.o
├── kernel/
│   └── task.o
├── drivers/
│   ├── tty.o
│   └── tty/
│       ├── uart/
│       │   └── pl011.o
│       └── tty_pl011.o
├── lib/
│   └── libc.a    (Static library)
└── init/
    └── main.o
```

## Library Building

Libraries are built separately using `ar` (archiver):

```makefile
lib/libc.a: obj/lib/libc/string/memcpy.o \
            obj/lib/libc/string/memset.o \
            obj/lib/libc/string/strcmp.o
    $(AR) rcs $@ $^
```

This creates a static archive that is linked during final executable generation.

## Compiler Flags Reference

### Common Flags

| Flag | Purpose |
|------|---------|
| `-march=armv8-a` | Target ARM v8-a architecture |
| `-Wall -Wextra` | Enable all warnings |
| `-Werror` | Treat warnings as errors |
| `-O2` | Optimization level 2 |
| `-g` | Include debug symbols |
| `-ffreestanding` | Kernel-style compilation (no standard library) |
| `-nostdlib` | Don't link standard C library |
| `-nostartfiles` | Don't include default startup code |

### Linker Flags

| Flag | Purpose |
|------|---------|
| `-T linker.ld` | Use custom linker script |
| `-Wl,--gc-sections` | Remove unused sections |
| `-nostdlib` | Don't search for standard libraries |
| `-Map=output.map` | Generate memory map |

## Build Customization

### Adding New Source Files

1. **Create source file** in appropriate directory:
   ```bash
   echo "#include <stdio.h>" > kernel/scheduler.c
   ```

2. **Add to submakefile** (e.g., `kernel/Makefile`):
   ```makefile
   LOCAL_OBJS := task.o scheduler.o
   ```

3. **Rebuild**:
   ```bash
   make clean
   make
   ```

### Adding New Subdirectory

1. **Create directory structure**:
   ```bash
   mkdir -p filesystem/{ext4,fat}
   ```

2. **Create subdirectory Makefile**:
   ```bash
   cp kernel/Makefile filesystem/Makefile
   # Edit to match new directory
   ```

3. **Update top-level Makefile**:
   ```makefile
   DIRS := arch/$(ARCH) kernel lib init drivers filesystem
   ```

4. **Rebuild**:
   ```bash
   make clean
   make
   ```

## Cross-Compilation

### Setting Compiler

Use environment variables or make variables:

```bash
# Method 1: Environment variables
export CC=aarch64-linux-gnu-gcc
export AR=aarch64-linux-gnu-ar
make

# Method 2: Make variables
make CC=aarch64-linux-gnu-gcc AR=aarch64-linux-gnu-ar

# Method 3: Modify config/default.config
# TRIPLET := aarch64-linux-gnu
```

### Verifying Cross-Compiler

```bash
aarch64-linux-gnu-gcc --version
aarch64-linux-gnu-ar --version
```

## Build Output

### Successful Build

```
make
...
    LD -T include/arch/aarch64/linker.ld ... obj/arch/aarch64/startup.o obj/kernel/task.o ... -o build/jangada
Target build/jangada build finished.
```

### Executable Information

```bash
file build/jangada
# build/jangada: ELF 64-bit LSB executable, ARM aarch64, version 1 (SYSV), statically linked, not stripped
```

## Troubleshooting

### Common Build Errors

| Error | Cause | Solution |
|-------|-------|----------|
| `aarch64-linux-gnu-gcc: command not found` | Cross-compiler not installed | Install with `apt-get install gcc-aarch64-linux-gnu` |
| `error: required variable TRIPLET not set` | Missing configuration | Set in `config/default.config` |
| `Makefile:123: undefined variable 'PROJECT_PATH'` | Configuration not sourced | Verify `include config/default.config` at top of Makefile |
| `collect2: fatal error: ld returned 1 exit status` | Linker error | Check linker script path and symbol definitions |

### Debugging Build

Enable verbose output:

```bash
# Show all make commands
make V=1

# Show executed gcc commands
make VERBOSE=1
```

Check generated map file:

```bash
make LDFLAGS="-T include/arch/aarch64/linker.ld -Map=build/kernel.map"
cat build/kernel.map
```

## Performance Optimization

### Compiler Optimization Levels

```makefile
# In config/make.global
CFLAGS := -O2 -march=armv8-a  # Default: level 2 optimization
CFLAGS := -O3 -march=armv8-a  # Aggressive optimization
CFLAGS := -Os -march=armv8-a  # Size optimization
```

### Section Garbage Collection

Already enabled by default:

```makefile
LDFLAGS := -Wl,--gc-sections   # Remove unused sections
```

This reduces final executable size by removing unreferenced code/data.

## Parallel Building

GNU Make supports parallel builds:

```bash
# Build with 4 parallel jobs
make -j4

# Build with as many jobs as CPU cores
make -j
```

## See Also

- [Getting Started](GETTING_STARTED.md) - How to build the project
- [Architecture](ARCHITECTURE.md) - Design overview
- [Drivers](DRIVERS.md) - Driver documentation
