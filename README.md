# 🛶 Jangada RTOS

> **“Lightweight, sturdy, and built for navigating rough waters — all good traits for an RTOS.”**

**Jangada RTOS** is a lightweight, modular, and efficient Real-Time Operating System designed for embedded systems that need reliability without the overhead.  
Inspired by the simplicity and resilience of a *jangada* - this RTOS is built to keep your systems afloat in the unpredictable seas of real-time constraints.

# Setup

To configure the project you will need to have:

 - menuconfig, an interactive TUI menu to configure options.
 - genconfig, to generate the config.h and .config file from the Kconfig

To install it execute the following command:

```bash
pip install kconfiglib
```

Confirm with:

```bash
which menuconfig
which genconfig
```

Test:

```bash
# Do some configurations

make menuconfig PROJECT_DIR=$HOME/Projects/Jangada-RTOS/examples/hello_world

# Compile
make PROJECT_DIR=$HOME/Projects/Jangada-RTOS/examples/hello_world 

# Run

qemu-system-aarch64 \
    -machine virt \
    -cpu cortex-a53 \
    -m 128M \
    -nographic \
    -kernel examples/hello_world/hello_world
```

