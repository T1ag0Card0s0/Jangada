First configure:

```bash
make menuconfig  -C ../.. PROJECT_DIR=$(pwd)
```

Compile:
```bash
make -C ../.. PROJECT_DIR=$(pwd)
```

To run

```bash 
qemu-system-aarch64 \
    -machine virt \
    -cpu cortex-a53 \
    -m 128M \
    -nographic \
    -kernel hello_world
```

