To build 
```bash
 cmake -B build -DBOARD=qemu_virt -DARCH=aarch64 --toolchain ../..
/cmake/toolchain/aarch64-none-elf.cmake
```

To run

```bash 
qemu-system-aarch64 \
    -machine virt \
    -cpu cortex-a53 \
    -m 128M \
    -nographic \
    -kernel build/hello_world.elf
```

