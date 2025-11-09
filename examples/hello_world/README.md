First configure:

```bash
make menuconfig
```

Compile:
```bash
make
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

